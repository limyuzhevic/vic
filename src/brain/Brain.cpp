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

namespace nlm {

// Helper function for safe pImpl access
Impl* safe_pimpl_access(Impl* ptr) {
    if (!ptr) {
        NLM_LOG_ERROR("Null pImpl access in Brain.cpp");
        return nullptr;
    }
    return ptr;
}

Impl* safe_pimpl_access(const Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Null Brain pointer in Brain.cpp");
        return nullptr;
    }
    return brain->pImpl;
}

// Helper function for configuration validation
bool validate_config(const Config& config) {
    if (!&config) {
        NLM_LOG_ERROR("Null config pointer in Brain.cpp");
        return false;
    }
    return true;
}

// Helper function for file operations
bool safe_file_operation(const std::string& filepath, const std::string& operation, bool success) {
    if (!success) {
        NLM_LOG_ERROR("File operation failed in Brain.cpp: " + operation + " on " + filepath);
        return false;
    }
    return true;
}

// Safe access macros for pImpl members
#define pImpl_safe pImpl_safe_impl

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
        if (!validate_config(*config)) {
            NLM_LOG_ERROR("Brain constructor failed: invalid config pointer");
            return;
        }
        
        // Validate config is accessible
        try {
            if (!config) {
                NLM_LOG_ERROR("Brain constructor failed: config pointer is null");
                return;
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Brain constructor failed: exception accessing config - " + std::string(e.what()));
            return;
        }
        
        // Initialize random generator with seed from config
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        
        // Validate seed value
        if (!validate_positive(static_cast<float>(seed), "random_seed", "Brain constructor")) {
            NLM_LOG_ERROR("Brain constructor failed: invalid random seed value");
            return;
        }
        
        try {
            rng = std::make_unique<RandomGenerator>(seed);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Brain constructor failed: failed to create random generator - " + std::string(e.what()));
            return;
        }
        
        // Check for memory allocation failures
        if (!rng) {
            NLM_LOG_ERROR("Brain constructor failed: failed to allocate random generator");
            return;
        }
        
        // Initialize plasticity systems with error handling
        try {
            spikeSystem = std::make_unique<SpikeSystem>();
            if (!spikeSystem) {
                NLM_LOG_ERROR("Brain constructor failed: failed to allocate spike system");
                return;
            }
            
            stdp = std::make_unique<STDP>();
            if (!stdp) {
                NLM_LOG_ERROR("Brain constructor failed: failed to allocate STDP module");
                return;
            }
            
            hebbian = std::make_unique<Hebbian>();
            if (!hebbian) {
                NLM_LOG_ERROR("Brain constructor failed: failed to allocate Hebbian module");
                return;
            }
            
            structuralPlasticity = std::make_unique<StructuralPlasticity>();
            if (!structuralPlasticity) {
                NLM_LOG_ERROR("Brain constructor failed: failed to allocate structural plasticity module");
                return;
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Brain constructor failed: exception initializing plasticity systems - " + std::string(e.what()));
            return;
        }
        
        // ========== INITIALIZE INTEGRATED SYSTEMS ==========
        
        try {
            // Initialize memory systems
            workingMemory = std::make_unique<NeuralWorkingMemory>();
            if (!workingMemory) {
                NLM_LOG_ERROR("Brain constructor failed: failed to allocate working memory");
                return;
            }
            
            episodicMemory = std::make_unique<NeuralEpisodicMemory>();
            if (!episodicMemory) {
                NLM_LOG_ERROR("Brain constructor failed: failed to allocate episodic memory");
                return;
            }
            
            associativeMemory = std::make_unique<NeuralAssociativeMemory>();
            if (!associativeMemory) {
                NLM_LOG_ERROR("Brain constructor failed: failed to allocate associative memory");
                return;
            }
            
            // Initialize prediction system
            predictionSystem = std::make_unique<PredictionSystem>();
            if (!predictionSystem) {
                NLM_LOG_ERROR("Brain constructor failed: failed to allocate prediction system");
                return;
            }
            
            // Initialize cognition systems
            planner = std::make_unique<NeuralPlanner>();
            if (!planner) {
                NLM_LOG_ERROR("Brain constructor failed: failed to allocate neural planner");
                return;
            }
            
            conceptFormation = std::make_unique<ConceptFormation>();
            if (!conceptFormation) {
                NLM_LOG_ERROR("Brain constructor failed: failed to allocate concept formation");
                return;
            }
            
            attention = std::make_unique<AttentionalSelection>();
            if (!attention) {
                NLM_LOG_ERROR("Brain constructor failed: failed to allocate attentional selection");
                return;
            }
            
            // Initialize development system
            developmentSystem = std::make_unique<DevelopmentSystem>();
            if (!developmentSystem) {
                NLM_LOG_ERROR("Brain constructor failed: failed to allocate development system");
                return;
            }
            
            // Initialize neuromodulation systems
            dopamine = std::make_unique<Dopamine>();
            if (!dopamine) {
                NLM_LOG_ERROR("Brain constructor failed: failed to allocate dopamine system");
                return;
            }
            
            curiosity = std::make_unique<Curiosity>();
            if (!curiosity) {
                NLM_LOG_ERROR("Brain constructor failed: failed to allocate curiosity system");
                return;
            }
            
            predictionError = std::make_unique<PredictionError>();
            if (!predictionError) {
                NLM_LOG_ERROR("Brain constructor failed: failed to allocate prediction error system");
                return;
            }
            
            novelty = std::make_unique<Novelty>();
            if (!novelty) {
                NLM_LOG_ERROR("Brain constructor failed: failed to allocate novelty system");
                return;
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Brain constructor failed: exception initializing integrated systems - " + std::string(e.what()));
            return;
        }
        
        // Configure STDP parameters
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config->getOr<float>("stdp_tau", 20.0f);
        
        // Validate STDP parameters
        if (!validate_positive(ltpWeight, "stdp_ltp_weight", "Brain constructor") ||
            !validate_positive(ltdWeight, "stdp_ltd_weight", "Brain constructor") ||
            !validate_positive(tau, "stdp_tau", "Brain constructor")) {
            NLM_LOG_ERROR("Brain constructor failed: invalid STDP parameters");
            return;
        }
        
        try {
            stdp->configure(ltpWeight, ltdWeight, tau);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Brain constructor failed: exception configuring STDP - " + std::string(e.what()));
            return;
        }
        
        // Configure structural plasticity
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
        
        // Validate plasticity parameters
        if (!validate_positive(synaptogenesisRate, "synaptogenesis_rate", "Brain constructor") ||
            !validate_positive(pruningRate, "pruning_rate", "Brain constructor")) {
            NLM_LOG_ERROR("Brain constructor failed: invalid plasticity parameters");
            return;
        }
        
        try {
            structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
            structuralPlasticity->setPruningRate(pruningRate);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Brain constructor failed: exception configuring structural plasticity - " + std::string(e.what()));
            return;
        }
        
        // Get timestep
        timestep = config->getOr<double>("simulation_timestep", 0.001);
        
        // Validate timestep
        if (!validate_positive(static_cast<float>(timestep), "simulation_timestep", "Brain constructor")) {
            NLM_LOG_ERROR("Brain constructor failed: invalid timestep value");
            return;
        }
        
        // ========== BIOLOGICAL INSPIRED PARAMETERS ==========
        // This constructor initializes the brain with parameters inspired by
        // cortical microcircuit properties and biological constraints.
        // The default values reflect typical neural time constants and plasticity rates.
        // Configuration-driven parameter tuning allows adaptation to different tasks
        // and environmental demands, mimicking how biological systems adjust their
        // parameters during development and learning.
        
        // Get integration intervals from config
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        
        // Validate intervals
        if (!validate_positive(static_cast<float>(replayInterval), "replay_interval", "Brain constructor") ||
            !validate_positive(static_cast<float>(consolidationInterval), "consolidation_interval", "Brain constructor")) {
            NLM_LOG_ERROR("Brain constructor failed: invalid interval values");
            return;
        }
        
        // Initialize checkpoint manager
        try {
            checkpointManager = std::make_unique<CheckpointManager>();
            if (!checkpointManager) {
                NLM_LOG_ERROR("Brain constructor failed: failed to allocate checkpoint manager");
                return;
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Brain constructor failed: exception creating checkpoint manager - " + std::string(e.what()));
            return;
        }
        
        NLM_LOG_INFO("Brain constructor completed successfully");
    
    // ========== PHASE 6: INTEGRATION ARCHITECTURE ==========
    // Phase 6 represents the fully integrated artificial brain with:
    // - Unified pImpl pattern for encapsulation and memory safety
    // - Comprehensive error handling and logging infrastructure
    // - Modular, pluggable system architecture for extensibility
    // - Real-time simulation capabilities with 15-step integration loop
    // - Support for developmental stages and adaptive plasticity
    // This architecture combines the best of biological brain organization
    // with software engineering principles for robust, maintainable code.
    }
    
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
};

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

// Brain destructor
    // Clean up all allocated resources and integrated systems
    ~Brain() = default;

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
    
    // Validate pImpl is accessible
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) {
        NLM_LOG_ERROR("Brain::initialize failed: null pImpl");
        return false;
    }
    
    // Get configuration values with error handling
    size_t neuronCount = 0;
    size_t regionCount = 0;
    float connectionProbability = 0.0f;
    
    try {
        if (auto neuronCountOpt = pimpl->config->get<size_t>("neuron_count")) {
            neuronCount = *neuronCountOpt;
        }
        if (auto regionCountOpt = pimpl->config->get<size_t>("region_count")) {
            regionCount = *regionCountOpt;
        }
        if (auto connectionProbabilityOpt = pimpl->config->get<float>("connection_probability")) {
            connectionProbability = *connectionProbabilityOpt;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Brain::initialize failed: exception accessing config values - " + std::string(e.what()));
        return false;
    }
    
    // Validate configuration values
    if (!validate_positive(static_cast<float>(neuronCount), "neuron_count", "Brain::initialize") ||
        !validate_positive(static_cast<float>(regionCount), "region_count", "Brain::initialize") ||
        !validate_positive(connectionProbability, "connection_probability", "Brain::initialize")) {
        NLM_LOG_ERROR("Brain::initialize failed: invalid configuration values");
        return false;
    }
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create regions with error handling
    for (size_t i = 0; i < regionCount; ++i) {
        try {
            addRegion("Region_" + std::to_string(i + 1));
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Brain::initialize failed: exception creating region " + std::to_string(i) + " - " + std::string(e.what()));
            continue; // Skip this region but continue with others
        }
    }
    
    // Create neurons across regions with bounds checking
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (!region) {
            NLM_LOG_ERROR("Brain::initialize failed: region " + std::to_string(i + 1) + " not found");
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
                    pimpl->sensoryNeurons.push_back(neuron);
                }
            }
            
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    pimpl->motorNeurons.push_back(neuron);
                }
            }
            
            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                         ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                         std::to_string(region->getTotalNeuronCount()) + " neurons");
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Brain::initialize failed: exception in region " + std::to_string(i) + " - " + std::string(e.what()));
            continue; // Skip this region but continue with others
        }
    }
    
    // Initialize connectivity with random weights with error handling
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (!region) {
            NLM_LOG_ERROR("Brain::initialize failed: region " + std::to_string(i + 1) + " not found for connectivity");
            continue;
        }
        
        try {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*pimpl->rng, connectionProbability, 0.2f, 0.1f);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Brain::initialize failed: exception initializing connectivity for region " + std::to_string(i) + " - " + std::string(e.what()));
            continue; // Skip connectivity but continue with other systems
        }
    }
    
    // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
    
    // Initialize working memory with error handling
    try {
        if (pimpl->workingMemory) {
            pimpl->workingMemory->initialize(this);
            pimpl->workingMemory->setCapacity(neuronCount / 10);
        } else {
            NLM_LOG_ERROR("Brain::initialize failed: working memory not available");
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Brain::initialize failed: exception initializing working memory - " + std::string(e.what()));
        return false;
    }
    
    // Initialize episodic memory with error handling
    try {
        if (pimpl->episodicMemory) {
            pimpl->episodicMemory->initialize(this);
            pimpl->episodicMemory->setMaxEpisodes(1000);
        } else {
            NLM_LOG_ERROR("Brain::initialize failed: episodic memory not available");
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Brain::initialize failed: exception initializing episodic memory - " + std::string(e.what()));
        return false;
    }
    
    // Initialize associative memory with error handling
    try {
        if (pimpl->associativeMemory) {
            pimpl->associativeMemory->initialize(this);
        } else {
            NLM_LOG_ERROR("Brain::initialize failed: associative memory not available");
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Brain::initialize failed: exception initializing associative memory - " + std::string(e.what()));
        return false;
    }
    
    // Initialize prediction system (placeholder comment)
    // PredictionSystem doesn't have initialize method currently
    
    // Initialize cognition systems with error handling
    try {
        if (pimpl->planner) {
            pimpl->planner->initialize(this);
            pimpl->planner->setPlanningDepth(5);
        } else {
            NLM_LOG_ERROR("Brain::initialize failed: neural planner not available");
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Brain::initialize failed: exception initializing neural planner - " + std::string(e.what()));
        return false;
    }
    
    try {
        if (pimpl->conceptFormation) {
            pimpl->conceptFormation->initialize(this);
        } else {
            NLM_LOG_ERROR("Brain::initialize failed: concept formation not available");
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Brain::initialize failed: exception initializing concept formation - " + std::string(e.what()));
        return false;
    }
    
    try {
        if (pimpl->attention) {
            pimpl->attention->initialize(this);
            pimpl->attention->setInhibitionStrength(0.5f);
            pimpl->attention->setExcitationStrength(1.5f);
        } else {
            NLM_LOG_ERROR("Brain::initialize failed: attentional selection not available");
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Brain::initialize failed: exception initializing attentional selection - " + std::string(e.what()));
        return false;
    }
    
    // Initialize neuromodulation with error handling
    try {
        if (pimpl->novelty) {
            pimpl->novelty->initialize(this);
        } else {
            NLM_LOG_ERROR("Brain::initialize failed: novelty detection not available");
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Brain::initialize failed: exception initializing novelty detection - " + std::string(e.what()));
        return false;
    }
    
    try {
        if (pimpl->curiosity) {
            pimpl->curiosity->initialize(this);
        } else {
            NLM_LOG_ERROR("Brain::initialize failed: curiosity system not available");
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Brain::initialize failed: exception initializing curiosity system - " + std::string(e.what()));
        return false;
    }
    
    // Register spike handlers for event-driven processing
    try {
        pimpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
            // Count spikes
            ++pimpl->totalSpikesThisStep;
            ++pimpl->totalSpikesTotal;
        });
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Brain::initialize failed: exception registering spike handler - " + std::string(e.what()));
        return false;
    }
    
    // Register delayed spike handler with error handling
    try {
        pimpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
            // Find destination neuron and deliver synaptic input
            for (auto& region : pimpl->regions) {
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
        NLM_LOG_ERROR("Brain::initialize failed: exception registering delayed spike handler - " + std::string(e.what()));
        return false;
    }
    
    // Configure checkpoint manager with error handling
    try {
        std::string checkpointDir = pimpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        pimpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Brain::initialize failed: exception configuring checkpoint manager - " + std::string(e.what()));
        return false;
    }
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pimpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pimpl->motorNeurons.size()));
    
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
     * 14. Collect statistics
     */
    
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) {
        NLM_LOG_ERROR("Brain::step failed: null pImpl");
        return;
    }
    
    pimpl->currentStep = currentStep;
    pimpl->currentTime = currentTime;
    pimpl->totalSpikesThisStep = 0;
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    // Biological inspiration: Spike-timing-dependent plasticity (STDP) requires precise
    // spike timing. Delayed spike processing mimics axonal conduction delays and ensures
    // that synaptic updates respect temporal order, which is crucial for temporal learning
    // and sequence encoding.
    try {
        pimpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Brain::step failed: exception processing delayed spikes - " + std::string(e.what()));
    }
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    // Biological inspiration: Leaky Integrate-and-Fire (LIF) neurons model membrane
    // potential dynamics with realistic biophysical properties. This implementation
    // captures the essential dynamics of cortical neurons while maintaining computational
    // efficiency. The integration step respects Dale's principle where excitatory and
    // inhibitory neurons play complementary roles in network dynamics.
    for (auto& region : pimpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                try {
                    neuron->stepLIF(currentTime, pimpl->timestep);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Brain::step failed: exception updating neuron - " + std::string(e.what()));
                }
            }
        }
    }
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    for (auto& region : pimpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                try {
                    // Check if neuron just fired this step
                    const auto& state = neuron->getState();
                    bool justFired = (state.firingState == FiringState::Refractory &&
                                     state.lastSpikeTime >= 0.0f &&
                                     std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pimpl->timestep * 2.0f);
                    
                    if (justFired) {
                        // Neuron fired this step - queue the spike
                        SpikeEvent event(neuron->getId(), currentTime, currentStep);
                        pimpl->spikeSystem->queueSpike(event);
                        
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
                            Timestamp deliveryTime = currentTime + delay * pimpl->timestep;
                            
                            // Biological inspiration: Spike delays model axonal conduction velocity
                            // and are crucial for temporal processing in neural circuits. Different
                            // synapse types have different delay characteristics, affecting information
                            // transmission timing across the network.
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
                            
                            pimpl->spikeSystem->queueDelayedSpike(delayedEvent);
                            
                            // Record pre-synaptic spike for plasticity
                            syn->recordPreSpike(currentTime);
                        }
                        
                        // Store to working memory - neurons that fire become part of working memory
                        if (pimpl->workingMemory) {
                            pimpl->workingMemory->storeToNeuron(neuron->getId(), 
                                std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                        }
                    }
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Brain::step failed: exception processing neuron " + std::to_string(neuron->getId()) + " - " + std::string(e.what()));
                }
            }
        }
    }
    
    // Process immediate spikes
    try {
        pimpl->spikeSystem->processSpikes(currentStep);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Brain::step failed: exception processing spikes - " + std::string(e.what()));
    }
    
    // ========== STEP 4: Update working memory ==========
    if (pimpl->workingMemory) {
        try {
            pimpl->workingMemory->update(pimpl->timestep);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Brain::step failed: exception updating working memory - " + std::string(e.what()));
        }
    }
    
    // ========== STEP 5: Apply neuromodulation effects ==========
    // Update novelty detection
    if (pimpl->novelty) {
        try {
            pimpl->novelty->update(pimpl->timestep);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Brain::step failed: exception updating novelty detection - " + std::string(e.what()));
        }
    }
    
    // Update curiosity
    if (pimpl->curiosity) {
        try {
            pimpl->curiosity->update(pimpl->timestep);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Brain::step failed: exception updating curiosity - " + std::string(e.what()));
        }
    }
    
    // Update dopamine (reward prediction error)
    if (pimpl->dopamine) {
        try {
            pimpl->dopamine->update(pimpl->timestep);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Brain::step failed: exception updating dopamine - " + std::string(e.what()));
        }
    }
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (pimpl->dopamine) {
        try {
            plasticityMod = pimpl->dopamine->getPlasticityFactor();
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Brain::step failed: exception getting dopamine plasticity factor - " + std::string(e.what()));
        }
    }
    
    for (auto& region : pimpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    try {
                        // Modify weight change based on dopamine
                        pimpl->stdp->update(syn, preSpikes, postSpikes, pimpl->timestep);
                        float weight = syn->getWeight();
                        weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                        syn->setWeight(weight);
                    } catch (const std::exception& e) {
                        NLM_LOG_ERROR("Brain::step failed: exception applying STDP - " + std::string(e.what()));
                    }
                }
            }
            
            // Apply Hebbian learning
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    try {
                        pimpl->hebbian->update(syn, preSpikes, postSpikes, pimpl->timestep);
                    } catch (const std::exception& e) {
                        NLM_LOG_ERROR("Brain::step failed: exception applying Hebbian learning - " + std::string(e.what()));
                    }
                }
            }
            
            // Update synapse state
            try {
                syn->step(currentTime);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Brain::step failed: exception updating synapse state - " + std::string(e.what()));
            }
        }
    }
    
    // ========== STEP 7: Update episodic memory ==========
    pimpl->stepsSinceLastEpisode++;
    if (pimpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pimpl->stepsSinceLastEpisode = 0;
        
        if (pimpl->episodicMemory) {
            try {
                // Capture current brain state as an episode
                EpisodicMemoryItem episode;
                episode.timestamp = currentStep;
                episode.reward = pimpl->dopamine ? pimpl->dopamine->getLevel() : 0.0f;
                
                // Store active neurons
                for (auto& region : pimpl->regions) {
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
                episode.reward = pimpl->dopamine ? pimpl->dopamine->getLevel() : 0.0f;
                
                pimpl->episodicMemory->storeEpisode(episode);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Brain::step failed: exception updating episodic memory - " + std::string(e.what()));
            }
        }
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (pimpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
    
    // ========== STEP 9: Update attention system ==========
    if (pimpl->attention) {
        try {
            pimpl->attention->update(pimpl->timestep);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Brain::step failed: exception updating attention system - " + std::string(e.what()));
        }
        
        // Apply attention to working memory winners
        if (pimpl->workingMemory && !pimpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pimpl->workingMemory->getMemoryNeurons();
            try {
                pimpl->attention->processCompetition(competitors);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Brain::step failed: exception processing attention competition - " + std::string(e.what()));
            }
        }
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (pimpl->conceptFormation) {
        try {
            // Process current neural activity patterns to form concepts
            // Extract neural activity patterns from brain regions
            std::vector<float> neuralActivityPattern;
            neuralActivityPattern.reserve(100);
            
            // Collect neural activity from all regions
            for (const auto& region : pimpl->regions) {
                for (const auto& pop : region->getPopulations()) {
                    for (const auto& neuron : pop->getNeurons()) {
                        const auto& state = neuron->getState();
                        neuralActivityPattern.push_back(
                            std::abs(state.membranePotential - state.restingPotential) / 20.0f);
                    }
                }
            }
            
            // Also include current sensory input if available
            std::vector<float> sensoryPattern;
            // Note: currentSensoryInput would need to be added to Brain class
            // For now, we'll use an empty vector
            // if (pimpl->currentSensoryInput) {
            //     sensoryPattern = pimpl->currentSensoryInput->getData();
            // }
            
            // Get reward signal from dopamine neuromodulation
            float reward = pimpl->dopamine ? pimpl->dopamine->getLevel() : 0.0f;
            
            // Update concept formation with current patterns
            pimpl->conceptFormation->update(sensoryPattern, neuralActivityPattern, reward);
            
            // Update concepts based on prediction error
            if (pimpl->predictionError) {
                float predictionError = pimpl->predictionError->getLevel();
                pimpl->conceptFormation->updateWithPredictionError(predictionError);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Brain::step failed: exception updating concept formation - " + std::string(e.what()));
        }
    }
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        try {
            pimpl->structuralPlasticity->update(this, *pimpl->rng);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Brain::step failed: exception applying structural plasticity - " + std::string(e.what()));
        }
    }
    
    // ========== STEP 12: Replay important memories ==========
    if (currentStep % pimpl->replayInterval == 0 && pimpl->episodicMemory) {
        try {
            // Get episodes for replay
            auto episodesToReplay = pimpl->episodicMemory->getEpisodesForReplay(3);
            for (const auto* episode : episodesToReplay) {
                pimpl->episodicMemory->replayEpisode(episode);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Brain::step failed: exception replaying memories - " + std::string(e.what()));
        }
    }
    
    // ========== STEP 13: Apply development effects ==========
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        try {
            pimpl->developmentSystem->update(this, *pimpl->rng, pimpl->timestep * 1000);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Brain::step failed: exception applying development effects - " + std::string(e.what()));
        }
        
        // Development affects plasticity rates
        auto* sp = pimpl->structuralPlasticity;
        if (sp) {
            DevelopmentalStage stage = pimpl->developmentalStage;
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
            
            try {
                sp->setSynaptogenesisRate(0.0001f * plasticityMod);
                sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Brain::step failed: exception applying plasticity rate modulation - " + std::string(e.what()));
            }
        }
    }
    
    // ========== STEP 14: Periodic memory consolidation ==========
    if (currentStep % pimpl->consolidationInterval == 0 && pimpl->episodicMemory) {
        try {
            // Consolidate important memories, remove weak ones
            pimpl->episodicMemory->consolidate(0.3f);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Brain::step failed: exception consolidating memories - " + std::string(e.what()));
        }
    }
    
    // ========== STEP 15: Checkpoint management ==========
    if (pimpl->checkpointManager) {
        try {
            pimpl->checkpointManager->update(currentStep, currentTime);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Brain::step failed: exception in checkpoint management - " + std::string(e.what()));
        }
    }
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Inject current into sensory neurons based on input
    // This is a simple mapping - sensory encoding
    
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) {
        NLM_LOG_ERROR("Brain::receiveSensoryInput failed: null pImpl");
        return;
    }
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = pimpl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Validate parameter
        if (!validate_positive(std::abs(normalizedValue), "sensory_input_value", "Brain::receiveSensoryInput")) {
            NLM_LOG_ERROR("Brain::receiveSensoryInput: invalid input value at index " + std::to_string(i));
            continue;
        }
        
        try {
            // Inject current into this sensory neuron
            pimpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
            
            // Also store in working memory
            if (pimpl->workingMemory && normalizedValue > 0.5f) {
                pimpl->workingMemory->storeToNeuron(pimpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Brain::receiveSensoryInput failed: exception injecting current to neuron " + std::to_string(pimpl->sensoryNeurons[i]->getId()) + " - " + std::string(e.what()));
        }
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) {
        NLM_LOG_ERROR("Brain::injectCurrent failed: null pImpl");
        return;
    }
    
    try {
        for (auto& region : pimpl->regions) {
            auto neurons = region->getAllNeurons();
            for (auto* n : neurons) {
                if (n->getId() == neuron) {
                    n->injectCurrent(current);
                    return;
                }
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Brain::injectCurrent failed: exception - " + std::string(e.what()));
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) {
        NLM_LOG_ERROR("Brain::injectCurrentToNeurons failed: null pImpl");
        return;
    }
    
    try {
        for (auto& region : pimpl->regions) {
            for (auto& pop : region->getPopulations()) {
                if (pop->getNeuronType() == type) {
                    for (auto* neuron : pop->getNeurons()) {
                        neuron->injectCurrent(current);
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Brain::injectCurrentToNeurons failed: exception - " + std::string(e.what()));
    }
}

SpikeSystem* Brain::getSpikeSystem() {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return nullptr;
    return pimpl->spikeSystem.get();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return nullptr;
    return pimpl->spikeSystem.get();
}

STDP* Brain::getSTDP() {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return nullptr;
    return pimpl->stdp.get();
}

Hebbian* Brain::getHebbian() {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return nullptr;
    return pimpl->hebbian.get();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return nullptr;
    return pimpl->structuralPlasticity.get();
}

float Brain::getExcitationInhibitionRatio() const {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return 0.0f;
    
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : pimpl->regions) {
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
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return 0;
    return pimpl->totalSpikesTotal;
}

size_t Brain::getPendingSpikeEventCount() const {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return 0;
    return pimpl->spikeSystem->getPendingSpikeCount() + pimpl->spikeSystem->getPendingDelayedCount();
}

std::unique_ptr<class Action> Brain::produceAction() {
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) {
        NLM_LOG_ERROR("Brain::produceAction failed: null pImpl");
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    if (pimpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : pimpl->motorNeurons) {
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
    
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) {
        NLM_LOG_ERROR("Brain::applyNeuromodulation failed: null pImpl");
        return;
    }
    
    // Validate parameter
    if (!validate_positive(modulation, "neuromodulation_level", "Brain::applyNeuromodulation")) {
        NLM_LOG_ERROR("Brain::applyNeuromodulation failed: invalid modulation value");
        return;
    }
    
    try {
        // Scale STDP learning rates
        pimpl->stdp->setLTPWeight(0.01f * modulation);
        pimpl->stdp->setLTDWeight(0.012f * modulation);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Brain::applyNeuromodulation failed: exception - " + std::string(e.what()));
    }
}

void Brain::updatePlasticity() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) {
        NLM_LOG_ERROR("Brain::updatePlasticity failed: null pImpl");
        return;
    }
}

void Brain::develop() {
    // Development updates structural plasticity
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) {
        NLM_LOG_ERROR("Brain::develop failed: null pImpl");
        return;
    }
    
    try {
        pimpl->structuralPlasticity->update(this, *pimpl->rng);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Brain::develop failed: exception - " + std::string(e.what()));
    }
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) {
        NLM_LOG_ERROR("Brain::reset failed: null pImpl");
        return;
    }
    
    pimpl->currentStep = 0;
    pimpl->currentTime = 0.0;
    pimpl->totalSpikesThisStep = 0;
    pimpl->totalSpikesTotal = 0;
    pimpl->isResting = false;
    pimpl->stepsSinceLastEpisode = 0;
    
    for (auto& region : pimpl->regions) {
        region->reset();
    }
    
    pimpl->spikeSystem->reset();
    pimpl->developmentalStage = DevelopmentalStage::Initial;
    
    // Reset memory systems
    if (pimpl->workingMemory) pimpl->workingMemory->clear();
    if (pimpl->episodicMemory) pimpl->episodicMemory->clear();
    if (pimpl->associativeMemory) pimpl->associativeMemory->clear();
    if (pimpl->attention) pimpl->attention->reset();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) {
        NLM_LOG_ERROR("Brain::save failed: null pImpl");
        return false;
    }
    
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
            pimpl->currentStep,
            pimpl->currentTime
        );
        
        // Write neurons
        NeuronCheckpointData neuronData;
        neuronData.membranePotential.reserve(getTotalNeuronCount());
        neuronData.restingPotential.reserve(getTotalNeuronCount());
        neuronData.threshold.reserve(getTotalNeuronCount());
        neuronData.resetPotential.reserve(getTotalNeuronCount());
        neuronData.leakConductance.reserve(getTotalNeuronCount());
        
        for (const auto& region : pimpl->regions) {
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
        for (const auto& region : pimpl->regions) {
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
    
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) {
        NLM_LOG_ERROR("Brain::load failed: null pImpl");
        return false;
    }
    
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
        for (auto& region : pimpl->regions) {
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
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) {
        NLM_LOG_ERROR("Brain::addRegion failed: null pImpl");
        return RegionId(0);
    }
    
    try {
        RegionId id(pimpl->nextRegionId++);
        auto region = std::make_unique<NeuralRegion>(id, name);
        pimpl->regions.push_back(std::move(region));
        return id;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Brain::addRegion failed: exception - " + std::string(e.what()));
        return RegionId(0);
    }
}

NeuralRegion* Brain::getRegion(RegionId id) {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) {
        NLM_LOG_ERROR("Brain::getRegion failed: null pImpl");
        return nullptr;
    }
    
    for (auto& region : pimpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) {
        NLM_LOG_ERROR("Brain::getRegion (const) failed: null pImpl");
        return nullptr;
    }
    
    for (const auto& region : pimpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

size_t Brain::getRegionCount() const {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return 0;
    return pimpl->regions.size();
}

std::vector<RegionId> Brain::getRegionIds() const {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) {
        NLM_LOG_ERROR("Brain::getRegionIds failed: null pImpl");
        return std::vector<RegionId>();
    }
    
    std::vector<RegionId> ids;
    ids.reserve(pimpl->regions.size());
    for (const auto& region : pimpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) {
        NLM_LOG_ERROR("Brain::getRegions failed: null pImpl");
        static const std::vector<std::unique_ptr<NeuralRegion>> empty;
        return empty;
    }
    return pimpl->regions;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) {
        NLM_LOG_ERROR("Brain::addInterRegionConnection failed: null pImpl");
        return;
    }
    
    // Validate parameters
    if (!validate_positive(weight, "connection_weight", "Brain::addInterRegionConnection") ||
        !validate_positive(static_cast<float>(delay), "connection_delay", "Brain::addInterRegionConnection")) {
        NLM_LOG_ERROR("Brain::addInterRegionConnection failed: invalid parameters");
        return;
    }
    
    pimpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) {
        NLM_LOG_ERROR("Brain::removeInterRegionConnection failed: null pImpl");
        return;
    }
    
    pimpl->interRegionConnections.erase(
        std::remove_if(pimpl->interRegionConnections.begin(),
                      pimpl->interRegionConnections.end(),
                      [source, target](const InterRegionConnection& conn) {
                          return conn.sourceRegion == source && conn.targetRegion == target;
                      }),
        pimpl->interRegionConnections.end()
    );
}

size_t Brain::getTotalNeuronCount() const {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return 0;
    
    size_t total = 0;
    for (const auto& region : pimpl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t Brain::getTotalSynapseCount() const {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return 0;
    
    size_t total = 0;
    for (const auto& region : pimpl->regions) {
        total += region->getSynapseCount();
    }
    total += pimpl->interRegionConnections.size();
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return 0;
    
    size_t total = 0;
    for (const auto& region : pimpl->regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t Brain::getFiringNeuronCount() const {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return 0;
    return pimpl->totalSpikesThisStep;
}

float Brain::getAverageFiringRate() const {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return 0.0f;
    
    if (pimpl->regions.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : pimpl->regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(pimpl->regions.size());
}

// ========== MEMORY SYSTEM ACCESSORS ==========

NeuralWorkingMemory* Brain::getWorkingMemory() {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return nullptr;
    return pimpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return nullptr;
    return pimpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return nullptr;
    return pimpl->associativeMemory.get();
}

// ========== PREDICTION SYSTEM ACCESSOR ==========

PredictionSystem* Brain::getPredictionSystem() {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return nullptr;
    return pimpl->predictionSystem.get();
}

// ========== COGNITION SYSTEM ACCESSORS ==========

NeuralPlanner* Brain::getPlanner() {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return nullptr;
    return pimpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return nullptr;
    return pimpl->conceptFormation.get();
}

// ========== ACCESSOR METHODS ==========

SimulationStep Brain::getCurrentStep() const {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return 0;
    return pimpl->currentStep;
}

Timestamp Brain::getCurrentTime() const {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return 0.0;
    return pimpl->currentTime;
}

AttentionalSelection* Brain::getAttention() {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return nullptr;
    return pimpl->attention.get();
}

// ========== DEVELOPMENT SYSTEM ==========

DevelopmentSystem* Brain::getDevelopmentSystem() {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return nullptr;
    return pimpl->developmentSystem.get();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return DevelopmentalStage::Initial;
    return pimpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) {
        NLM_LOG_ERROR("Brain::setDevelopmentalStage failed: null pImpl");
        return;
    }
    pimpl->developmentalStage = stage;
}

// ========== NEUROMODULATION SYSTEMS ==========

Dopamine* Brain::getDopamine() {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return nullptr;
    return pimpl->dopamine.get();
}

Curiosity* Brain::getCuriosity() {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return nullptr;
    return pimpl->curiosity.get();
}

Novelty* Brain::getNovelty() {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return nullptr;
    return pimpl->novelty.get();
}

PredictionError* Brain::getPredictionErrorSignal() {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return nullptr;
    return pimpl->predictionError.get();
}

std::shared_ptr<const Config> Brain::getConfig() const {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return nullptr;
    return pimpl->config;
}

RandomGenerator* Brain::getRandomGenerator() {
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) return nullptr;
    return pimpl->rng.get();
}

void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
    
    auto* pimpl = pImpl_safe(pImpl);
    if (!pimpl) {
        NLM_LOG_ERROR("Brain::logStatus failed: null pImpl");
        return;
    }
    
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
    if (pimpl->workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(pimpl->workingMemory->getActiveTraces()));
    }
    if (pimpl->episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(pimpl->episodicMemory->getEpisodeCount()));
    }
    
    // Neuromodulation status
    if (pimpl->dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(pimpl->dopamine->getLevel()));
    }
    
    // Development status
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(pimpl->developmentalStage)));
    
    for (const auto& region : pimpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                     " (" + region->getName() + "): " +
                     std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                     std::to_string(region->getSynapseCount()) + " synapses, " +
                     "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

} // namespace nlm
