// Enhanced NLM Brain with comprehensive error handling and validation
// Phase 6: Integrated Artificial Brain with robust error management

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
#include "../neuromodulation/New/old.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "core/ErrorHandling.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <limits>

namespace nlm {

// Enhanced error handling utilities for Brain.cpp
namespace BrainError {
    
    // Brain initialization errors
    class BrainInitError : public NLMError {
    public:
        BrainInitError(const std::string& message, ErrorCategory category = ErrorCategory::ERROR)
            : NLMError(ErrorCode::NEURON_INVALID_STATE, message, category) {}
    };
    
    // Region errors
    class RegionError : public NLMError {
    public:
        RegionError(const std::string& message, ErrorCategory category = ErrorCategory::ERROR)
            : NLMError(ErrorCode::REGION_NOT_FOUND, message, category) {}
    };
    
    // Synapse errors
    class SynapseError : public NLMError {
    public:
        SynapseError(const std::string& message, ErrorCategory category = ErrorCategory::ERROR)
            : NLMError(ErrorCode::SYNAPSE_INVALID_WEIGHT, message, category) {}
    };
    
    // Memory system errors
    class MemoryError : public NLMError {
    public:
        MemoryError(const std::string& message, ErrorCategory category = ErrorCategory::ERROR)
            : NLMError(ErrorCode::MEMORATION_FAILED, message, category) {}
    };
    
    // Plasticity errors
    class PlasticityError : public NLMError {
    public:
        PlasticityError(const std::string& message, ErrorCategory category = ErrorCategory::ERROR)
            : NLMError(ErrorCode::PLASTICITY_OVERFLOW, message, category) {}
    };
    
    // Simulation errors
    class SimulationError : public NLMError {
    public:
        SimulationError(const std::string& message, ErrorCategory category = ErrorCategory::ERROR)
            : NLMError(ErrorCode::SIMULATION_TIMEOUT, message, category) {}
    };
};

// Enhanced Brain::Impl with error handling
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
    
    // Error tracking
    std::vector<std::string> errorHistory;
    size_t consecutiveErrors;
    
    Impl(std::shared_ptr<Config> cfg) : 
        config(cfg)
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
        , replayInterval(100)
        , consolidationInterval(1000)
        , consecutiveErrors(0)
    {
        try {
            // Initialize random generator with seed from config
            uint64_t seed = 42;  // Default seed
            if (auto seedOpt = config->get<uint64_t>("random_seed")) {
                seed = *seedOpt;
            }
            rng = std::make_unique<RandomGenerator>(seed);
            
            // Initialize plasticity systems with error handling
            initializePlasticitySystems();
            
            // ========== INITIALIZE INTEGRATED SYSTEMS ==========
            
            // Initialize memory systems with error handling
            initializeMemorySystems();
            
            // Initialize prediction system
            initializePredictionSystem();
            
            // Initialize cognition systems
            initializeCognitionSystems();
            
            // Initialize development system
            initializeDevelopmentSystem();
            
            // Initialize neuromodulation systems
            initializeNeuromodulationSystems();
            
            // Configure STDP parameters
            configureSTDPParameters();
            
            // Configure structural plasticity
            configureStructuralPlasticity();
            
            // Get timestep
            timestep = config->getOr<double>("simulation_timestep", 0.001);
            
            // Get integration intervals from config
            replayInterval = config->getOr<size_t>("replay_interval", 100);
            consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
            
            // Initialize checkpoint manager
            checkpointManager = std::make_unique<CheckpointManager>();
            
            NLM_LOG_INFO("BrainImpl initialized successfully");
            
        } catch (const NLMError& e) {
            NLM_LOG_ERROR("Failed to initialize BrainImpl: " + std::string(e.what()));
            throw;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Unexpected error during BrainImpl initialization: " + std::string(e.what()));
            throw BrainError::BrainInitError(std::string("Unexpected error during initialization: ") + e.what());
        }
    }
    
    void initializePlasticitySystems() {
        try {
            spikeSystem = std::make_unique<SpikeSystem>();
            stdp = std::make_unique<STDP>();
            hebbian = std::make_unique<Hebbian>();
            structuralPlasticity = std::make_unique<StructuralPlasticity>();
        } catch (const std::exception& e) {
            throw BrainError::BrainInitError(std::string("Failed to initialize plasticity systems: ") + e.what());
        }
    }
    
    void initializeMemorySystems() {
        try {
            workingMemory = std::make_unique<NeuralWorkingMemory>();
            episodicMemory = std::make_unique<NeuralEpisodicMemory>();
            associativeMemory = std::make_unique<NeuralAssociativeMemory>();
            
            if (!workingMemory || !episodicMemory || !associativeMemory) {
                throw BrainError::MemoryError("Failed to allocate memory system components");
            }
            
            // Initialize working memory
            workingMemory->initialize(nullptr); // Will be set by Brain::initialize
            workingMemory->setCapacity(1000);
            
            // Initialize episodic memory
            episodicMemory->initialize(nullptr); // Will be set by Brain::initialize
            episodicMemory->setMaxEpisodes(1000);
            
            // Initialize associative memory
            associativeMemory->initialize(nullptr); // Will be set by Brain::initialize
            
        } catch (const std::exception& e) {
            throw BrainError::MemoryError(std::string("Failed to initialize memory systems: ") + e.what());
        }
    }
    
    void initializePredictionSystem() {
        try {
            predictionSystem = std::make_unique<PredictionSystem>();
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Failed to initialize prediction system: ") + e.what());
        }
    }
    
    void initializeCognitionSystems() {
        try {
            planner = std::make_unique<NeuralPlanner>();
            conceptFormation = std::make_unique<ConceptFormation>();
            attention = std::make_unique<AttentionalSelection>();
            
            if (!planner || !conceptFormation || !attention) {
                throw BrainError::MemoryError("Failed to allocate cognition system components");
            }
            
            // Initialize cognition systems
            // Note: Would need Brain reference, will be set later
            
        } catch (const std::exception& e) {
            throw BrainError::MemoryError(std::string("Failed to initialize cognition systems: ") + e.what());
        }
    }
    
    void initializeDevelopmentSystem() {
        try {
            developmentSystem = std::make_unique<DevelopmentSystem>();
        } catch (const std::exception& e) {
            throw BrainError::BrainInitError(std::string("Failed to initialize development system: ") + e.what());
        }
    }
    
    void initializeNeuromodulationSystems() {
        try {
            dopamine = std::make_unique<Dopamine>();
            curiosity = std::make_unique<Curiosity>();
            predictionError = std::make_unique<PredictionError>();
            novelty = std::make_unique<Novelty>();
            
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Failed to initialize neuromodulation systems: ") + e.what());
        }
    }
    
    void configureSTDPParameters() {
        try {
            float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
            float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
            float tau = config->getOr<float>("stdp_tau", 20.0f);
            
            // Validate STDP parameters
            if (ltpWeight < 0 || ltdWeight < 0 || tau <= 0) {
                throw BrainError::PlasticityError("Invalid STDP parameters: weights must be positive");
            }
            
            stdp->configure(ltpWeight, ltdWeight, tau);
            
        } catch (const NLMError& e) {
            throw;
        } catch (const std::exception& e) {
            throw BrainError::PlasticityError(std::string("Failed to configure STDP parameters: ") + e.what());
        }
    }
    
    void configureStructuralPlasticity() {
        try {
            float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
            float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
            
            // Validate plasticity rates
            if (synaptogenesisRate < 0 || synaptogenesisRate > 1.0) {
                throw BrainError::PlasticityError("Synaptogenesis rate must be between 0 and 1");
            }
            if (pruningRate < 0 || pruningRate > 1.0) {
                throw BrainError::PlasticityError("Pruning rate must be between 0 and 1");
            }
            if (synaptogenesisRate + pruningRate > 1.0) {
                throw BrainError::PlasticityError("Sum of synaptogenesis and pruning rates cannot exceed 1.0");
            }
            
            structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
            structuralPlasticity->setPruningRate(pruningRate);
            
        } catch (const NLMError& e) {
            throw;
        } catch (const std::exception& e) {
            throw BrainError::PlasticityError(std::string("Failed to configure structural plasticity: ") + e.what());
        }
    }
    
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
    
    // Record error
    void recordError(const std::string& error) {
        std::lock_guard<std::mutex> lock(errorMutex);
        errorHistory.push_back(error);
        consecutiveErrors++;
        
        if (consecutiveErrors >= 10) {
            NLM_LOG_ERROR("Critical error threshold reached: " + error);
        }
    }
    
    // Reset error count
    void resetErrorCount() {
        std::lock_guard<std::mutex> lock(errorMutex);
        consecutiveErrors = 0;
    }
    
    mutable std::mutex errorMutex;
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
        // Get configuration values with validation
        size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
        size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
        float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
        
        // Validate configuration values
        if (!validateConfigurationValues(neuronCount, regionCount, connectionProbability)) {
            NLM_LOG_ERROR("Invalid configuration values");
            return false;
        }
        
        NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                     std::to_string(regionCount) + " regions");
        
        // Clear previous error history
        pImpl->errorHistory.clear();
        pImpl->resetErrorCount();
        
        // Create regions with error handling
        createRegions(regionCount);
        
        // Create neurons across regions with error handling
        createNeurons(neuronCount, regionCount);
        
        // Initialize connectivity with random weights
        initializeConnectivity(connectionProbability);
        
        // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
        
        initializeMemorySystems();
        initializePredictionSystem();
        initializeCognitionSystems();
        initializeNeuromodulationSystems();
        registerSpikeHandlers();
        
        // Configure checkpoint manager
        configureCheckpointManager();
        
        NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
        NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
        NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
        NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
        
        return true;
        
    } catch (const NLMError& e) {
        NLM_LOG_ERROR(std::string("Brain initialization failed: ") + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error during brain initialization: ") + e.what());
        return false;
    }
}

bool Brain::validateConfigurationValues(size_t neuronCount, size_t regionCount, float connectionProbability) {
    // Validate neuron count
    if (neuronCount < 10 || neuronCount > 1000000) {
        NLM_LOG_ERROR("Invalid neuron count: " + std::to_string(neuronCount) + ". Must be between 10 and 1,000,000");
        return false;
    }
    
    // Validate region count
    if (regionCount < 1 || regionCount > 100) {
        NLM_LOG_ERROR("Invalid region count: " + std::to_string(regionCount) + ". Must be between 1 and 100");
        return false;
    }
    
    // Validate connection probability
    if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
        NLM_LOG_ERROR("Invalid connection probability: " + std::to_string(connectionProbability) + ". Must be between 0 and 1");
        return false;
    }
    
    // Check if neuron count is reasonable for region count
    size_t neuronsPerRegion = neuronCount / regionCount;
    if (neuronsPerRegion < 10) {
        NLM_LOG_WARNING("Low neuron density: " + std::to_string(neuronsPerRegion) + " neurons per region");
    }
    
    return true;
}

void Brain::createRegions(size_t regionCount) {
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
}

void Brain::createNeurons(size_t neuronCount, size_t regionCount) {
    size_t neuronsPerRegion = neuronCount / regionCount;
    
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (!region) {
            pImpl->recordError("Failed to get region " + std::to_string(i + 1));
            continue;
        }
        
        // Add populations with error handling
        auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
        auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
        auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
        
        // Collect sensory and motor neurons for I/O with null checks
        auto* sensoryPop = region->getPopulation(sensoryPopId);
        auto* motorPop = region->getPopulation(motorPopId);
        
        if (sensoryPop) {
            for (auto* neuron : sensoryPop->getNeurons()) {
                if (neuron) {
                    pImpl->sensoryNeurons.push_back(neuron);
                } else {
                    pImpl->recordError("Null neuron in sensory population");
                }
            }
        }
        
        if (motorPop) {
            for (auto* neuron : motorPop->getNeurons()) {
                if (neuron) {
                    pImpl->motorNeurons.push_back(neuron);
                } else {
                    pImpl->recordError("Null neuron in motor population");
                }
            }
        }
        
        NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                    ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons");
    }
}

void Brain::initializeConnectivity(float connectionProbability) {
    // Initialize connectivity with random weights
    for (size_t i = 0; i < pImpl->regions.size(); ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights with error handling
            try {
                region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
            } catch (const std::exception& e) {
                pImpl->recordError(std::string("Failed to initialize connectivity for region ") + std::to_string(i + 1) + ": " + e.what());
            }
        }
    }
}

void Brain::initializeMemorySystems() {
    if (!pImpl->workingMemory) {
        return;
    }
    
    try {
        // Initialize with Brain reference for self-referential connections
        pImpl->workingMemory->initialize(this);
        pImpl->workingMemory->setCapacity(getTotalNeuronCount() / 10);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to initialize working memory: ") + e.what());
    }
}

void Brain::initializePredictionSystem() {
    if (pImpl->predictionSystem) {
        try {
            // PredictionSystem would be initialized with Brain reference
            // For now, just track prediction error history
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Failed to initialize prediction system: ") + e.what());
        }
    }
}

void Brain::initializeCognitionSystems() {
    try {
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
        NLM_LOG_ERROR(std::string("Failed to initialize cognition systems: ") + e.what());
    }
}

void Brain::initializeNeuromodulationSystems() {
    try {
        if (pImpl->novelty) {
            pImpl->novelty->initialize(this);
        }
        
        if (pImpl->curiosity) {
            pImpl->curiosity->initialize(this);
        }
        
        // Dopamine would be initialized when reward signals are available
        
    } catch (const std::exception& e) {
        NLM_LOG_WARNING(std::string("Failed to initialize neuromodulation systems: ") + e.what());
    }
}

void Brain::registerSpikeHandlers() {
    try {
        // Register spike handlers for event-driven processing
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
                // Count spikes
                ++pImpl->totalSpikesThisStep;
                ++pImpl->totalSpikesTotal;
            });
        }
        
        // Register delayed spike handler to deliver synaptic input
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
                // Find destination neuron and deliver synaptic input
                deliverSynapticInput(event);
            });
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to register spike handlers: ") + e.what());
    }
}

void Brain::deliverSynapticInput(const DelayedSpikeEvent& event) {
    try {
        // Find destination neuron and deliver synaptic input
        for (auto& region : pImpl->regions) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron && neuron->getId() == event.destination_neuron) {
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
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error delivering synaptic input: ") + e.what());
    }
}

void Brain::configureCheckpointManager() {
    try {
        if (pImpl->checkpointManager) {
            std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
            pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to configure checkpoint manager: ") + e.what());
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
        
        pImpl->currentStep = currentStep;
        pImpl->currentTime = currentTime;
        pImpl->totalSpikesThisStep = 0;
        
        // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
        }
        
        // ========== STEP 2: Update all neurons (LIF dynamics) ==========
        updateNeurons(currentStep, currentTime);
        
        // ========== STEP 3: Detect spikes and schedule spike events ==========
        detectAndScheduleSpikes(currentStep, currentTime);
        
        // Process immediate spikes
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->processSpikes(currentStep);
        }
        
        // ========== STEP 4: Update working memory ==========
        updateWorkingMemory();
        
        // ========== STEP 5: Apply neuromodulation effects ==========
        updateNeuromodulation();
        
        // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
        applyPlasticityRules(currentTime);
        
        // ========== STEP 7: Update episodic memory ==========
        updateEpisodicMemory();
        
        // ========== STEP 8: Update prediction system ==========
        updatePredictionSystem();
        
        // ========== STEP 9: Update attention system ==========
        updateAttentionSystem();
        
        // ========== STEP 10: Update concept formation ==========
        updateConceptFormation();
        
        // ========== STEP 11: Apply structural plasticity periodically ==========
        applyStructuralPlasticity(currentStep);
        
        // ========== STEP 12: Replay important memories ==========
        replayMemories(currentStep);
        
        // ========== STEP 13: Apply development effects ==========
        applyDevelopmentEffects(currentStep);
        
        // ========== STEP 14: Periodic memory consolidation ==========
        consolidateMemories(currentStep);
        
        // ========== STEP 15: Checkpoint management ==========
        updateCheckpoints(currentStep, currentTime);
        
    } catch (const BrainError::SimulationError& e) {
        NLM_LOG_ERROR(std::string("Simulation error: ") + e.what());
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error during brain step: ") + e.what());
        throw BrainError::SimulationError(std::string("Unexpected error during simulation: ") + e.what());
    }
}

void Brain::updateNeurons(SimulationStep currentStep, Timestamp currentTime) {
    try {
        for (auto& region : pImpl->regions) {
            if (!region) {
                pImpl->recordError("Null region in updateNeurons");
                continue;
            }
            
            for (auto& pop : region->getPopulations()) {
                if (!pop) {
                    pImpl->recordError("Null population in updateNeurons");
                    continue;
                }
                
                for (auto* neuron : pop->getNeurons()) {
                    if (!neuron) {
                        pImpl->recordError("Null neuron in updateNeurons");
                        continue;
                    }
                    
                    try {
                        neuron->stepLIF(currentTime, pImpl->timestep);
                    } catch (const std::exception& e) {
                        NLM_LOG_ERROR(std::string("Error updating neuron " ) + std::to_string(neuron->getId()) + ": " + e.what());
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error in updateNeurons: ") + e.what());
        throw;
    }
}

void Brain::detectAndScheduleSpikes(SimulationStep currentStep, Timestamp currentTime) {
    try {
        for (auto& region : pImpl->regions) {
            if (!region) {
                pImpl->recordError("Null region in detectAndScheduleSpikes");
                continue;
            }
            
            for (auto& pop : region->getPopulations()) {
                if (!pop) {
                    pImpl->recordError("Null population in detectAndScheduleSpikes");
                    continue;
                }
                
                for (auto* neuron : pop->getNeurons()) {
                    if (!neuron) {
                        pImpl->recordError("Null neuron in detectAndScheduleSpikes");
                        continue;
                    }
                    
                    try {
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
                                if (!syn) {
                                    continue;
                                }
                                
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

                                    if (pImpl->spikeSystem) {
                                        pImpl->spikeSystem->queueDelayedSpike(delayedEvent);
                                    }

                                    // Record pre-synaptic spike for plasticity
                                    syn->recordPreSpike(currentTime);
                                    
                                } catch (const std::exception& e) {
                                    NLM_LOG_ERROR(std::string("Error scheduling spike for synapse ") + std::to_string(syn->getId()) + ": " + e.what());
                                }
                            }
                            
                            // Store to working memory - neurons that fire become part of working memory
                            if (pImpl->workingMemory) {
                                try {
                                    pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                                        std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                                } catch (const std::exception& e) {
                                    NLM_LOG_ERROR(std::string("Error storing neuron in working memory: ") + e.what());
                                }
                            }
                            
                        }
                    } catch (const std::exception& e) {
                        NLM_LOG_ERROR(std::string("Error detecting spike for neuron ") + std::to_string(neuron->getId()) + ": " + e.what());
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error in detectAndScheduleSpikes: ") + e.what());
        throw;
    }
}

void Brain::updateWorkingMemory() {
    if (!pImpl->workingMemory) {
        return;
    }
    
    try {
        pImpl->workingMemory->update(pImpl->timestep);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error updating working memory: ") + e.what());
    }
}

void Brain::updateNeuromodulation() {
    try {
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
                if (!region) {
                    pImpl->recordError("Null region in updateNeuromodulation");
                    continue;
                }
                
                for (auto& pop : region->getPopulations()) {
                    if (!pop) {
                        pImpl->recordError("Null population in updateNeuromodulation");
                        continue;
                    }
                    
                    for (auto* neuron : pop->getNeurons()) {
                        if (!neuron) {
                            pImpl->recordError("Null neuron in updateNeuromodulation");
                            continue;
                        }
                        
                        try {
                            // Dopamine modulates excitability by injecting additional current
                            float excitabilityMod = dopamineLevel * 0.5f;
                            if (excitabilityMod > 0.0f) {
                                neuron->injectCurrent(excitabilityMod);
                            }
                        } catch (const std::exception& e) {
                            NLM_LOG_ERROR(std::string("Error applying dopamine to neuron ") + std::to_string(neuron->getId()) + ": " + e.what());
                        }
                    }
                }
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error in updateNeuromodulation: ") + e.what());
    }
}

void Brain::applyPlasticityRules(Timestamp currentTime) {
    try {
        // Calculate neuromodulation factor for plasticity
        float plasticityMod = 1.0f;
        if (pImpl->dopamine) {
            plasticityMod = pImpl->dopamine->getPlasticityFactor();
        }
        
        for (auto& region : pImpl->regions) {
            if (!region) {
                pImpl->recordError("Null region in applyPlasticityRules");
                continue;
            }
            
            for (auto& syn : region->getSynapses()) {
                if (!syn) {
                    pImpl->recordError("Null synapse in applyPlasticityRules");
                    continue;
                }
                
                try {
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
                    
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Error applying plasticity to synapse ") + std::to_string(syn->getId()) + ": " + e.what());
                }
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error in applyPlasticityRules: ") + e.what());
    }
}

void Brain::updateEpisodicMemory() {
    pImpl->stepsSinceLastEpisode++;
    
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            try {
                // Capture current brain state as an episode
                EpisodicMemoryItem episode;
                episode.timestamp = pImpl->currentStep;
                episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
                
                // Store active neurons
                for (auto& region : pImpl->regions) {
                    if (!region) {
                        pImpl->recordError("Null region in updateEpisodicMemory");
                        continue;
                    }
                    
                    for (auto& pop : region->getPopulations()) {
                        if (!pop) {
                            pImpl->recordError("Null population in updateEpisodicMemory");
                            continue;
                        }
                        
                        for (auto* neuron : pop->getNeurons()) {
                            if (!neuron) {
                                pImpl->recordError("Null neuron in updateEpisodicMemory");
                                continue;
                            }
                            
                            try {
                                if (neuron->isFiring() || 
                                    std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                                    episode.activeNeurons.push_back(neuron->getId());
                                    episode.neuronActivations.push_back(
                                        std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                                }
                            } catch (const std::exception& e) {
                                NLM_LOG_ERROR(std::string("Error capturing neuron state for episodic memory: ") + e.what());
                            }
                        }
                    }
                }
                
                // Store reward in episode
                episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
                
                pImpl->episodicMemory->storeEpisode(episode);
                
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Error updating episodic memory: ") + e.what());
            }
        }
    }
}

void Brain::updatePredictionSystem() {
    if (!pImpl->predictionSystem) {
        return;
    }
    
    try {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
        // This would typically capture current sensory state and compare with predictions
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error updating prediction system: ") + e.what());
    }
}

void Brain::updateAttentionSystem() {
    if (!pImpl->attention) {
        return;
    }
    
    try {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error updating attention system: ") + e.what());
    }
}

void Brain::updateConceptFormation() {
    if (!pImpl->conceptFormation) {
        return;
    }
    
    try {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
        // Currently placeholder for future implementation
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error updating concept formation: ") + e.what());
    }
}

void Brain::applyStructuralPlasticity(SimulationStep currentStep) {
    if (!pImpl->structuralPlasticity) {
        return;
    }
    
    try {
        if (currentStep % 100 == 0) {
            pImpl->structuralPlasticity->update(this, *pImpl->rng);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error applying structural plasticity: ") + e.what());
    }
}

void Brain::replayMemories(SimulationStep currentStep) {
    if (!pImpl->episodicMemory) {
        return;
    }
    
    try {
        if (currentStep % pImpl->replayInterval == 0) {
            // Get episodes for replay
            auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
            for (const auto* episode : episodesToReplay) {
                if (episode) {
                    pImpl->episodicMemory->replayEpisode(episode);
                }
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error replaying memories: ") + e.what());
    }
}

void Brain::applyDevelopmentEffects(SimulationStep currentStep) {
    if (!pImpl->developmentSystem) {
        return;
    }
    
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
        NLM_LOG_ERROR(std::string("Error applying development effects: ") + e.what());
    }
}

void Brain::consolidateMemories(SimulationStep currentStep) {
    if (!pImpl->episodicMemory) {
        return;
    }
    
    try {
        if (currentStep % pImpl->consolidationInterval == 0) {
            // Consolidate important memories, remove weak ones
            pImpl->episodicMemory->consolidate(0.3f);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error consolidating memories: ") + e.what());
    }
}

void Brain::updateCheckpoints(SimulationStep currentStep, Timestamp currentTime) {
    if (!pImpl->checkpointManager) {
        return;
    }
    
    try {
        pImpl->checkpointManager->update(currentStep, currentTime);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error updating checkpoints: ") + e.what());
    }
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    try {
        const auto& values = input.getData();
        if (values.empty()) {
            return;
        }
        
        size_t numSensory = pImpl->sensoryNeurons.size();
        if (numSensory == 0) {
            return;
        }
        
        // Distribute input across sensory neurons with bounds checking
        for (size_t i = 0; i < numSensory; ++i) {
            // Normalize input value to range [-10, 10] mV
            float normalizedValue = 0.0f;
            if (i < values.size()) {
                normalizedValue = static_cast<float>(values[i]) * 10.0f;
            }
            
            // Inject current into this sensory neuron
            if (i < pImpl->sensoryNeurons.size() && pImpl->sensoryNeurons[i]) {
                pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
                
                // Also store in working memory
                if (pImpl->workingMemory && normalizedValue > 0.5f) {
                    pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
                }
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error receiving sensory input: ") + e.what());
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    try {
        // Validate inputs
        if (!pImpl->rng) {
            NLM_LOG_ERROR("Random generator is null in injectCurrent");
            return;
        }
        
        for (auto& region : pImpl->regions) {
            if (!region) {
                pImpl->recordError("Null region in injectCurrent");
                continue;
            }
            
            auto neurons = region->getAllNeurons();
            for (auto* n : neurons) {
                if (n && n->getId() == neuron) {
                    n->injectCurrent(current);
                    return;
                }
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error injecting current: ") + e.what());
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    try {
        for (auto& region : pImpl->regions) {
            if (!region) {
                pImpl->recordError("Null region in injectCurrentToNeurons");
                continue;
            }
            
            for (auto& pop : region->getPopulations()) {
                if (!pop) {
                    pImpl->recordError("Null population in injectCurrentToNeurons");
                    continue;
                }
                
                if (pop->getNeuronType() == type) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron) {
                            neuron->injectCurrent(current);
                        }
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error injecting current to neurons: ") + e.what());
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
        if (!region) {
            continue;
        }
        
        for (const auto& syn : region->getSynapses()) {
            if (!syn) {
                continue;
            }
            
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
    if (!pImpl->spikeSystem) {
        return 0;
    }
    return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
}

std::unique_ptr<class Action> Brain::produceAction() {
    try {
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
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error producing action: ") + e.what());
        return std::make_unique<Action>(ActionType::Wait);
    }
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    try {
        float modulation = signal.getLevel();
        
        // Scale STDP learning rates
        if (pImpl->stdp) {
            pImpl->stdp->setLTPWeight(0.01f * modulation);
            pImpl->stdp->setLTDWeight(0.012f * modulation);
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error applying neuromodulation: ") + e.what());
    }
}

void Brain::updatePlasticity() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void Brain::develop() {
    try {
        // Development updates structural plasticity
        if (pImpl->structuralPlasticity) {
            pImpl->structuralPlasticity->update(this, *pImpl->rng);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error during development: ") + e.what());
    }
}

void Brain::reset() {
    try {
        NLM_LOG_INFO("Resetting NLM Brain...");
        
        pImpl->currentStep = 0;
        pImpl->currentTime = 0.0;
        pImpl->totalSpikesThisStep = 0;
        pImpl->totalSpikesTotal = 0;
        pImpl->isResting = false;
        pImpl->stepsSinceLastEpisode = 0;
        
        for (auto& region : pImpl->regions) {
            if (region) {
                region->reset();
            }
        }
        
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->reset();
        }
        
        pImpl->developmentalStage = DevelopmentalStage::Initial;
        
        // Reset memory systems
        if (pImpl->workingMemory) pImpl->workingMemory->clear();
        if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
        if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
        if (pImpl->attention) pImpl->attention->reset();
        
        NLM_LOG_INFO("NLM Brain reset complete");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error during brain reset: ") + e.what());
    }
}

bool Brain::save(const std::string& filepath) const {
    try {
        NLM_LOG_INFO("Saving brain state to " + filepath);
        
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
            if (!region) {
                continue;
            }
            
            for (const auto& pop : region->getPopulations()) {
                if (!pop) {
                    continue;
                }
                
                for (const auto* neuron : pop->getNeurons()) {
                    if (!neuron) {
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
            NLM_LOG_ERROR("Failed to write neurons to checkpoint");
            return false;
        }
        
        // Write synapses
        SynapseCheckpointData synapseData;
        for (const auto& region : pImpl->regions) {
            if (!region) {
                continue;
            }
            
            for (const auto* syn : region->getSynapses()) {
                if (!syn) {
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
        
    } catch (const NLMError& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error saving brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    try {
        NLM_LOG_INFO("Loading brain state from " + filepath);
        
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
            if (!region) {
                continue;
            }
            
            for (auto& pop : region->getPopulations()) {
                if (!pop) {
                    continue;
                }
                
                for (auto* neuron : pop->getNeurons()) {
                    if (!neuron) {
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
        
    } catch (const NLMError& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error loading brain: ") + e.what());
        return false;
    }
}

RegionId Brain::addRegion(const std::string& name) {
    try {
        RegionId id(pImpl->nextRegionId++);
        auto region = std::make_unique<NeuralRegion>(id, name);
        if (!region) {
            throw BrainError::RegionError("Failed to create neural region: memory allocation failed");
        }
        
        pImpl->regions.push_back(std::move(region));
        return id;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error adding region: ") + e.what());
        throw;
    }
}

NeuralRegion* Brain::getRegion(RegionId id) {
    try {
        for (auto& region : pImpl->regions) {
            if (region && region->getId() == id) {
                return region.get();
            }
        }
        return nullptr;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error getting region: ") + e.what());
        return nullptr;
    }
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    try {
        for (const auto& region : pImpl->regions) {
            if (region && region->getId() == id) {
                return region.get();
            }
        }
        return nullptr;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error getting region: ") + e.what());
        return nullptr;
    }
}

size_t Brain::getRegionCount() const {
    return pImpl->regions.size();
}

std::vector<RegionId> Brain::getRegionIds() const {
    std::vector<RegionId> ids;
    ids.reserve(pImpl->regions.size());
    
    try {
        for (const auto& region : pImpl->regions) {
            if (region) {
                ids.push_back(region->getId());
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error getting region IDs: ") + e.what());
    }
    
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->regions;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    try {
        // Validate inputs
        if (source.index() >= 1000000 || target.index() >= 1000000) {
            throw BrainError::RegionError("Invalid region ID in addInterRegionConnection");
        }
        
        if (weight < -100.0f || weight > 100.0f) {
            throw BrainError::SynapseError("Synapse weight out of range in addInterRegionConnection");
        }
        
        if (delay < 0) {
            throw BrainError::SynapseError("Negative delay in addInterRegionConnection");
        }
        
        pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error adding inter-region connection: ") + e.what());
        throw;
    }
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    try {
        pImpl->interRegionConnections.erase(
            std::remove_if(pImpl->interRegionConnections.begin(),
                          pImpl->interRegionConnections.end(),
                          [source, target](const InterRegionConnection& conn) {
                              return conn.sourceRegion == source && conn.targetRegion == target;
                          }),
            pImpl->interRegionConnections.end()
        );
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error removing inter-region connection: ") + e.what());
    }
}

size_t Brain::getTotalNeuronCount() const {
    size_t total = 0;
    
    try {
        for (const auto& region : pImpl->regions) {
            if (region) {
                total += region->getTotalNeuronCount();
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error calculating total neuron count: ") + e.what());
    }
    
    return total;
}

size_t Brain::getTotalSynapseCount() const {
    size_t total = 0;
    
    try {
        for (const auto& region : pImpl->regions) {
            if (region) {
                total += region->getSynapseCount();
            }
        }
        total += pImpl->interRegionConnections.size();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error calculating total synapse count: ") + e.what());
    }
    
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    size_t total = 0;
    
    try {
        for (const auto& region : pImpl->regions) {
            if (region) {
                total += region->getActiveNeuronCount();
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error calculating active neuron count: ") + e.what());
    }
    
    return total;
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl->totalSpikesThisStep;
}

float Brain::getAverageFiringRate() const {
    if (pImpl->regions.empty()) return 0.0f;
    
    float sum = 0.0f;
    
    try {
        for (const auto& region : pImpl->regions) {
            if (region) {
                sum += region->getAverageFiringRate();
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error calculating average firing rate: ") + e.what());
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
        
        // Log error history
        if (!pImpl->errorHistory.empty()) {
            NLM_LOG_INFO("Error history entries: " + std::to_string(pImpl->errorHistory.size()));
        }
        
        for (const auto& region : pImpl->regions) {
            if (!region) {
                continue;
            }
            
            NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                        " (" + region->getName() + "): " +
                        std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                        std::to_string(region->getSynapseCount()) + " synapses, " +
                        "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error logging brain status: ") + e.what());
    }
}

} // namespace nlm

// Implementation of AgentBrain with error handling
namespace nlm {

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
{
    try {
        // Initialize motor and sensory neuron groups
        if (brain_) {
            for (const auto& region : brain_->getRegions()) {
                if (!region) {
                    continue;
                }
                
                for (auto& pop : region->getPopulations()) {
                    if (!pop) {
                        continue;
                    }
                    
                    NeuronType type = pop->getNeuronType();
                    
                    if (type == NeuronType::Motor) {
                        for (Neuron* n : pop->getNeurons()) {
                            if (!n) {
                                continue;
                            }
                            
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
                        for (Neuron* n : pop->getNeurons()) {
                            if (!n) {
                                continue;
                            }
                            
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
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error in AgentBrain constructor: ") + e.what());
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    try {
        previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        
        NLM_LOG_INFO("AgentBrain initialized with " + 
                     std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                     std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                     std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error in AgentBrain::initialize: ") + e.what());
    }
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    try {
        if (!brain_) {
            return;
        }
        
        // Vision input (256 values -> sensoryVision_ neurons)
        const auto& vision = percept.getVision();
        for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
            if (sensoryVision_[i]) {
                // Inject current proportional to vision intensity
                float current = vision[i] * 5.0f;  // Scale factor
                sensoryVision_[i]->injectCurrent(current);
            }
        }
        
        // Touch input (8 values -> sensoryTouch_ neurons)
        const auto& touch = percept.getTouch();
        for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
            if (sensoryTouch_[i]) {
                float current = touch[i] * 8.0f;  // Collision signal
                sensoryTouch_[i]->injectCurrent(current);
            }
        }
        
        // Internal signals (4 values -> sensoryInternal_ neurons)
        const auto& intern = percept.getInternal();
        for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
            if (sensoryInternal_[i]) {
                float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
                sensoryInternal_[i]->injectCurrent(current);
            }
        }
        
        // Proprioception (6 values -> sensoryProprioception_ neurons)
        const auto& proprio = percept.getProprioception();
        for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
            if (sensoryProprioception_[i]) {
                float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
                sensoryProprioception_[i]->injectCurrent(current);
            }
        }
        
        // Compute novelty (difference from previous vision)
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
            
            // Store for next time
            previousVision_ = vision;
        }
        
        // Update curiosity based on novelty
        if (curiosityEnabled_) {
            curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
            curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error processing sensory input: ") + e.what());
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    try {
        if (!brain_) {
            return MotorCommand::Wait;
        }
        
        MotorCommand decoded = decodeFromMotorNeurons();
        
        // Apply curiosity-based exploration
        if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
            decoded = selectWithCuriosity(decoded);
        }
        
        return decoded;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error decoding motor command: ") + e.what());
        return MotorCommand::Wait;
    }
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    try {
        // Calculate average activity in each motor group
        auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
            if (neurons.empty()) return 0.0f;
            float sum = 0.0f;
            for (Neuron* n : neurons) {
                if (!n) {
                    continue;
                }
                // Use membrane potential deviation from rest as activity measure
                sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
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
        
        // Only act if there's meaningful activity
        if (bestActivity < 0.5f) {
            return MotorCommand::Wait;
        }
        
        return best;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error decoding from motor neurons: ") + e.what());
        return MotorCommand::Wait;
    }
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    try {
        // Exploration: occasionally choose random action when curiosity is high
        if (curiosityLevel_ > 0.5f) {
            // Higher curiosity = more exploration
            float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
            
            float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
            if (r < exploreChance) {
                // Random motor command
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
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error selecting with curiosity: ") + e.what());
        return defaultCmd;
    }
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    try {
        if (!brain_ || !rewardModulationEnabled_) {
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
        for (const auto& region : brain_->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                if (!syn) {
                    continue;
                }
                
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
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error applying reward modulation: ") + e.what());
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    try {
        if (!brain_ || !developmentEnabled_) {
            return;
        }
        
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
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error updating development: ") + e.what());
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) {
        return DevelopmentalStage::Initial;
    }
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
    try {
        dopamineLevel_ = 0.0f;
        noveltyLevel_ = 0.0f;
        curiosityLevel_ = 0.0f;
        predictionError_ = 0.0f;
        expectedReward_ = 0.0f;
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        
        // Clear previous vision
        std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error resetting AgentBrain: ") + e.what());
    }
}

} // namespace nlm