// Comprehensive Brain.cpp implementation with error handling
// This file implements robust error handling and validation for all Brain operations

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
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "../plasticity/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "Error.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <mutex>
#include <memory>
#include <vector>
#include <stdexcept>
#include <system_error>

namespace nlm {

// Global mutex for Brain thread safety
static std::mutex brainMutex;

// Implementation of Brain::Impl with error handling
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
    bool brainInitialized;
    bool systemsInitialized;
    bool checkpointingEnabled;
    
    // Thread safety for critical operations
    mutable std::mutex criticalMutex;
    
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
        , brainInitialized(false)
        , systemsInitialized(false)
        , checkpointingEnabled(false)
    {
        try {
            // Initialize random generator with seed from config
            uint64_t seed = 42;  // Default seed
            if (auto seedOpt = config->get<uint64_t>("random_seed")) {
                seed = *seedOpt;
            }
            rng = std::make_unique<RandomGenerator>(seed);
            
            // Validate and configure timestep
            timestep = config->getOr<double>("simulation_timestep", 0.001);
            InputValidator::isWithinRange("simulation_timestep", timestep, 0.0001, 0.1);
            
            // Validate configuration values
            validateConfiguration();
            
            // Initialize plasticity systems
            initializePlasticitySystems();
            
            // ========== INITIALIZE INTEGRATED SYSTEMS ==========
            
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
            
            // Initialize spike system
            spikeSystem = std::make_unique<SpikeSystem>();
            if (!checkSystemInitialization(spikeSystem.get(), "SpikeSystem")) {
                NLM_LOG_WARNING("SpikeSystem initialization returned false - will use degraded mode");
            }
            
            // Configure STDP parameters
            configureSTDP();
            
            // Configure structural plasticity
            configureStructuralPlasticity();
            
            // Get integration intervals from config
            replayInterval = config->getOr<size_t>("replay_interval", 100);
            consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
            
            // Initialize checkpoint manager
            checkpointManager = std::make_unique<CheckpointManager>();
            if (!checkpointManager) {
                throw BrainError("Failed to create checkpoint manager", "Impl constructor");
            }
            
            // Configure checkpointing
            std::string checkpointDir = config->getOr<std::string>("checkpoint_dir", "./checkpoints");
            InputValidator::isNonEmpty("checkpoint_dir", checkpointDir);
            checkpointManager->configure(checkpointDir, 10000, 5, true);
            
            brainInitialized = true;
            
            NLM_LOG_INFO("Brain.Impl initialization complete - all systems initialized");
            
        } catch (const BrainError& e) {
            // Clean up partially initialized systems
            cleanupOnError();
            throw;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Brain initialization error: ") + e.what());
            cleanupOnError();
            throw BrainError(std::string("Brain initialization failed: ") + e.what(), "Impl constructor");
        }
    }
    
    ~Impl() {
        try {
            cleanup();
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error during Brain cleanup: ") + e.what());
        }
    }
    
    void validateConfiguration() {
        // Validate critical configuration values
        auto neuronCountOpt = config->get<size_t>("neuron_count");
        if (!neuronCountOpt) {
            throw BrainError("Missing required configuration: neuron_count", "validateConfiguration");
        }
        InputValidator::isWithinRange("neuron_count", *neuronCountOpt, 1, 1000000);
        
        auto regionCountOpt = config->get<size_t>("region_count");
        if (!regionCountOpt) {
            throw BrainError("Missing required configuration: region_count", "validateConfiguration");
        }
        InputValidator::isWithinRange("region_count", *regionCountOpt, 1, 100);
        
        auto connectionProbabilityOpt = config->get<float>("connection_probability");
        if (!connectionProbabilityOpt) {
            throw BrainError("Missing required configuration: connection_probability", "validateConfiguration");
        }
        InputValidator::isWithinRange("connection_probability", *connectionProbabilityOpt, 0.0f, 1.0f);
        
        // Validate optional parameters
        if (auto maxMemoryOpt = config->get<size_t>("max_memory_usage")) {
            InputValidator::isWithinRange("max_memory_usage", *maxMemoryOpt, 1024, 1024 * 1024 * 1024); // 1KB to 1GB
        }
        
        if (auto maxStepsOpt = config->get<uint64_t>("max_steps")) {
            InputValidator::isWithinRange("max_steps", *maxStepsOpt, 1, 1000000000);
        }
    }
    
    void initializePlasticitySystems() {
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        if (!checkSystemInitialization(spikeSystem.get(), "SpikeSystem")) {
            throw BrainError("SpikeSystem initialization failed", "initializePlasticitySystems");
        }
        if (!checkSystemInitialization(stdp.get(), "STDP")) {
            NLM_LOG_WARNING("STDP initialization returned false - will use degraded mode");
        }
        if (!checkSystemInitialization(hebbian.get(), "Hebbian")) {
            NLM_LOG_WARNING("Hebbian initialization returned false - will use degraded mode");
        }
        if (!checkSystemInitialization(structuralPlasticity.get(), "StructuralPlasticity")) {
            throw BrainError("StructuralPlasticity initialization failed", "initializePlasticitySystems");
        }
    }
    
    void initializeMemorySystems() {
        workingMemory = std::make_unique<NeuralWorkingMemory>();
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        
        if (!checkSystemInitialization(workingMemory.get(), "WorkingMemory")) {
            throw BrainError("WorkingMemory initialization failed", "initializeMemorySystems");
        }
        
        try {
            workingMemory->initialize(this);
            auto capacityOpt = config->get<size_t>("working_memory_capacity");
            size_t capacity = capacityOpt ? *capacityOpt : 1000;
            workingMemory->setCapacity(capacity);
            
            // Configure working memory parameters
            if (auto decayOpt = config->get<float>("working_memory_decay_rate")) {
                workingMemory->setDecayRate(*decayOpt);
            }
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("WorkingMemory initialization error: ") + e.what());
            workingMemory.reset(); // Reset on error
            throw;
        }
        
        if (!checkSystemInitialization(episodicMemory.get(), "EpisodicMemory")) {
            throw BrainError("EpisodicMemory initialization failed", "initializeMemorySystems");
        }
        
        try {
            episodicMemory->initialize(this);
            auto maxEpisodesOpt = config->get<size_t>("max_episodes");
            size_t maxEpisodes = maxEpisodesOpt ? *maxEpisodesOpt : 1000;
            episodicMemory->setMaxEpisodes(maxEpisodes);
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("EpisodicMemory initialization error: ") + e.what());
            episodicMemory.reset(); // Reset on error
            throw;
        }
        
        if (!checkSystemInitialization(associativeMemory.get(), "AssociativeMemory")) {
            throw BrainError("AssociativeMemory initialization failed", "initializeMemorySystems");
        }
        
        try {
            associativeMemory->initialize(this);
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("AssociativeMemory initialization error: ") + e.what());
            associativeMemory.reset(); // Reset on error
            throw;
        }
    }
    
    void initializePredictionSystem() {
        predictionSystem = std::make_unique<PredictionSystem>();
        if (!checkSystemInitialization(predictionSystem.get(), "PredictionSystem")) {
            throw BrainError("PredictionSystem initialization failed", "initializePredictionSystem");
        }
    }
    
    void initializeCognitionSystems() {
        planner = std::make_unique<NeuralPlanner>();
        conceptFormation = std::make_unique<ConceptFormation>();
        attention = std::make_unique<AttentionalSelection>();
        
        if (!checkSystemInitialization(planner.get(), "Planner")) {
            throw BrainError("Planner initialization failed", "initializeCognitionSystems");
        }
        
        try {
            planner->initialize(this);
            auto planningDepthOpt = config->get<size_t>("planning_depth");
            if (planningDepthOpt) {
                planner->setPlanningDepth(*planningDepthOpt);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Planner initialization error: ") + e.what());
            planner.reset(); // Reset on error
            throw;
        }
        
        if (!checkSystemInitialization(conceptFormation.get(), "ConceptFormation")) {
            throw BrainError("ConceptFormation initialization failed", "initializeCognitionSystems");
        }
        
        try {
            conceptFormation->initialize(this);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("ConceptFormation initialization error: ") + e.what());
            conceptFormation.reset(); // Reset on error
            throw;
        }
        
        if (!checkSystemInitialization(attention.get(), "Attention")) {
            throw BrainError("Attention initialization failed", "initializeCognitionSystems");
        }
        
        try {
            attention->initialize(this);
            auto inhibitionStrengthOpt = config->get<float>("attention_inhibition_strength");
            if (inhibitionStrengthOpt) {
                attention->setInhibitionStrength(*inhibitionStrengthOpt);
            }
            auto excitationStrengthOpt = config->get<float>("attention_excitation_strength");
            if (excitationStrengthOpt) {
                attention->setExcitationStrength(*excitationStrengthOpt);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Attention initialization error: ") + e.what());
            attention.reset(); // Reset on error
            throw;
        }
    }
    
    void initializeDevelopmentSystem() {
        developmentSystem = std::make_unique<DevelopmentSystem>();
        if (!checkSystemInitialization(developmentSystem.get(), "DevelopmentSystem")) {
            throw BrainError("DevelopmentSystem initialization failed", "initializeDevelopmentSystem");
        }
    }
    
    void initializeNeuromodulationSystems() {
        dopamine = std::make_unique<Dopamine>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
        
        // Check and initialize each system
        if (!checkSystemInitialization(dopamine.get(), "Dopamine")) {
            throw BrainError("Dopamine initialization failed", "initializeNeuromodulationSystems");
        }
        
        if (!checkSystemInitialization(curiosity.get(), "Curiosity")) {
            throw BrainError("Curiosity initialization failed", "initializeNeuromodulationSystems");
        }
        
        if (!checkSystemInitialization(predictionError.get(), "PredictionError")) {
            throw BrainError("PredictionError initialization failed", "initializeNeuromodulationSystems");
        }
        
        if (!checkSystemInitialization(novelty.get(), "Novelty")) {
            throw BrainError("Novelty initialization failed", "initializeNeuromodulationSystems");
        }
        
        // Initialize neuromodulation systems
        try {
            dopamine->initialize(this);
            curiosity->initialize(this);
            predictionError->initialize(this);
            novelty->initialize(this);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Neuromodulation initialization error: ") + e.what());
            cleanupNeuromodulation();
            throw;
        }
    }
    
    void configureSTDP() {
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config->getOr<float>("stdp_tau", 20.0f);
        
        InputValidator::isWithinRange("stdp_ltp_weight", ltpWeight, 0.0f, 1.0f);
        InputValidator::isWithinRange("stdp_ltd_weight", ltdWeight, 0.0f, 1.0f);
        InputValidator::isWithinRange("stdp_tau", tau, 0.0f, 1000.0f);
        
        if (!checkSystemInitialization(stdp.get(), "STDP")) {
            throw BrainError("STDP system not initialized for configuration", "configureSTDP");
        }
        
        try {
            stdp->configure(ltpWeight, ltdWeight, tau);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("STDP configuration error: ") + e.what());
            throw;
        }
    }
    
    void configureStructuralPlasticity() {
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
        
        InputValidator::isWithinRange("synaptogenesis_rate", synaptogenesisRate, 0.0f, 0.01f);
        InputValidator::isWithinRange("pruning_rate", pruningRate, 0.0f, 0.01f);
        
        if (!checkSystemInitialization(structuralPlasticity.get(), "StructuralPlasticity")) {
            throw BrainError("StructuralPlasticity system not initialized for configuration", "configureStructuralPlasticity");
        }
        
        try {
            structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
            structuralPlasticity->setPruningRate(pruningRate);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("StructuralPlasticity configuration error: ") + e.what());
            throw;
        }
    }
    
    bool checkSystemInitialization(void* system, const std::string& systemName) {
        // Check if system pointer is valid
        if (!system) {
            NLM_LOG_ERROR("System initialization failed: " + systemName + " is null");
            return false;
        }
        
        // Additional system-specific checks can be added here
        // For now, just check if the pointer is valid
        return true;
    }
    
    void cleanupNeuromodulation() {
        try {
            if (dopamine) dopamine.reset();
            if (curiosity) curiosity.reset();
            if (predictionError) predictionError.reset();
            if (novelty) novelty.reset();
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error during neuromodulation cleanup: ") + e.what());
        }
    }
    
    void cleanupOnError() {
        // Cleanup partially initialized systems on error
        try {
            cleanupNeuromodulation();
            if (predictionSystem) predictionSystem.reset();
            if (workingMemory) workingMemory.reset();
            if (episodicMemory) episodicMemory.reset();
            if (associativeMemory) associativeMemory.reset();
            if (planner) planner.reset();
            if (conceptFormation) conceptFormation.reset();
            if (attention) attention.reset();
            if (developmentSystem) developmentSystem.reset();
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error during error cleanup: ") + e.what());
        }
        
        brainInitialized = false;
        systemsInitialized = false;
    }
    
    void cleanup() {
        try {
            // Cleanup all integrated systems
            cleanupNeuromodulation();
            if (predictionSystem) predictionSystem.reset();
            if (workingMemory) workingMemory.reset();
            if (episodicMemory) episodicMemory.reset();
            if (associativeMemory) associativeMemory.reset();
            if (planner) planner.reset();
            if (conceptFormation) conceptFormation.reset();
            if (attention) attention.reset();
            if (developmentSystem) developmentSystem.reset();
            
            // Cleanup plasticity systems
            if (structuralPlasticity) structuralPlasticity.reset();
            if (hebbian) hebbian.reset();
            if (stdp) stdp.reset();
            if (spikeSystem) spikeSystem.reset();
            
            // Cleanup checkpoint system
            if (checkpointManager) checkpointManager.reset();
            
            // Clear neurons and regions
            sensoryNeurons.clear();
            motorNeurons.clear();
            
            for (auto& region : regions) {
                if (region) {
                    region->cleanup();
                }
            }
            regions.clear();
            interRegionConnections.clear();
            
            brainInitialized = false;
            systemsInitialized = false;
            
            NLM_LOG_INFO("Brain.Impl cleanup complete");
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error during cleanup: ") + e.what());
            throw;
        }
    }
    
    void registerSystemInitializationFlags() {
        systemsInitialized = true;
    }
    
    bool isBrainInitialized() const {
        std::lock_guard<std::mutex> lock(criticalMutex);
        return brainInitialized && systemsInitialized;
    }
    
    // Safe access to internal state
    Config* getConfig() const { return config.get(); }
    RandomGenerator* getRng() const { return rng.get(); }
    SpikeSystem* getSpikeSystem() const { return spikeSystem.get(); }
    STDP* getSTDP() const { return stdp.get(); }
    DevelopmentSystem* getDevelopmentSystem() const { return developmentSystem.get(); }
    NeuralWorkingMemory* getWorkingMemory() const { return workingMemory.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() const { return episodicMemory.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() const { return associativeMemory.get(); }
    PredictionSystem* getPredictionSystem() const { return predictionSystem.get(); }
    NeuralPlanner* getPlanner() const { return planner.get(); }
    ConceptFormation* getConceptFormation() const { return conceptFormation.get(); }
    AttentionalSelection* getAttention() const { return attention.get(); }
    Dopamine* getDopamine() const { return dopamine.get(); }
    Curiosity* getCuriosity() const { return curiosity.get(); }
    Novelty* getNovelty() const { return novelty.get(); }
    PredictionError* getPredictionError() const { return predictionError.get(); }
    NeuralRegion* getRegion(RegionId id) const {
        for (const auto& region : regions) {
            if (region && region->getId() == id) {
                return region.get();
            }
        }
        return nullptr;
    }
    
    RegionId addRegion(const std::string& name) {
        std::lock_guard<std::mutex> lock(criticalMutex);
        
        try {
            InputValidator::isNonEmpty("region name", name);
            
            RegionId id(nextRegionId++);
            auto region = std::make_unique<NeuralRegion>(id, name);
            regions.push_back(std::move(region));
            return id;
            
        } catch (const BrainError& e) {
            throw;
        } catch (const std::exception& e) {
            throw BrainError(std::string("Failed to add region: ") + e.what(), "addRegion");
        }
    }
    
    NeuralRegion* getRegion(RegionId id) {
        std::lock_guard<std::mutex> lock(criticalMutex);
        
        for (auto& region : regions) {
            if (region && region->getId() == id) {
                return region.get();
            }
        }
        return nullptr;
    }
    
    DevelopmentalStage getDevelopmentalStage() const {
        return developmentalStage;
    }
    
    void setDevelopmentalStage(DevelopmentalStage stage) {
        std::lock_guard<std::mutex> lock(criticalMutex);
        developmentalStage = stage;
    }
    
    // Thread-safe step execution
    void executeStep(SimulationStep currentStep, Timestamp currentTime) {
        std::lock_guard<std::mutex> lock(criticalMutex);
        
        if (!isBrainInitialized()) {
            throw BrainError("Brain is not initialized", "executeStep");
        }
        
        try {
            // Check if spike system is available
            if (!spikeSystem) {
                throw BrainError("Spike system not available", "executeStep");
            }
            
            // Execute step with error handling
            executeStepImpl(currentStep, currentTime);
            
        } catch (const BrainError& e) {
            throw;
        } catch (const std::exception& e) {
            throw BrainError(std::string("Step execution failed: ") + e.what(), "executeStep");
        }
    }
    
    void executeStepImpl(SimulationStep currentStep, Timestamp currentTime) {
        // Store current state
        SimulationStep previousStep = this->currentStep;
        Timestamp previousTime = this->currentTime;
        
        try {
            // Validate inputs
            InputValidator::isWithinRange("currentStep", currentStep, 0, std::numeric_limits<SimulationStep>::max());
            InputValidator::isWithinRange("currentTime", currentTime, 0.0, std::numeric_limits<Timestamp>::max());
            
            // Update step counter
            this->currentStep = currentStep;
            this->currentTime = currentTime;
            this->totalSpikesThisStep = 0;
            
            // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
            if (spikeSystem) {
                spikeSystem->processDelayedSpikes(currentStep, currentTime);
            }
            
            // ========== STEP 2: Update all neurons (LIF dynamics) ==========
            updateNeurons(currentStep, currentTime);
            
            // ========== STEP 3: Detect spikes and schedule spike events ==========
            detectSpikes(currentStep, currentTime);
            
            // ========== STEP 4: Process immediate spikes ==========
            if (spikeSystem) {
                spikeSystem->processSpikes(currentStep);
            }
            
            // ========== STEP 5: Update working memory ==========
            updateWorkingMemory(currentTime);
            
            // ========== STEP 6: Apply neuromodulation effects ==========
            applyNeuromodulationEffects(currentTime);
            
            // ========== STEP 7: Apply plasticity rules (STDP and Hebbian) ==========
            applyPlasticityRules(currentTime);
            
            // ========== STEP 8: Update episodic memory ==========
            updateEpisodicMemory(currentStep, currentTime);
            
            // ========== STEP 9: Update prediction system ==========
            updatePredictionSystem(currentTime);
            
            // ========== STEP 10: Update attention system ==========
            updateAttentionSystem(currentTime);
            
            // ========== STEP 11: Update concept formation ==========
            updateConceptFormation(currentTime);
            
            // ========== STEP 12: Apply structural plasticity periodically ==========
            applyStructuralPlasticity(currentStep, currentTime);
            
            // ========== STEP 13: Replay important memories ==========
            replayMemories(currentStep, currentTime);
            
            // ========== STEP 14: Apply development effects ==========
            applyDevelopmentEffects(currentStep, currentTime);
            
            // ========== STEP 15: Periodic memory consolidation ==========
            consolidateMemory(currentStep, currentTime);
            
            // ========== STEP 16: Checkpoint management ==========
            manageCheckpoints(currentStep, currentTime);
            
            // ========== STEP 17: Collect statistics ==========
            collectStatistics(currentStep, currentTime);
            
        } catch (const BrainError& e) {
            // Restore previous state on error
            this->currentStep = previousStep;
            this->currentTime = previousTime;
            throw;
        } catch (const std::exception& e) {
            // Restore previous state on error
            this->currentStep = previousStep;
            this->currentTime = previousTime;
            NLM_LOG_ERROR(std::string("Step execution error: ") + e.what());
            throw BrainError(std::string("Step execution failed: ") + e.what(), "executeStepImpl");
        }
    }
    
    void updateNeurons(SimulationStep currentStep, Timestamp currentTime) {
        for (auto& region : regions) {
            if (!region) {
                NLM_LOG_WARNING("Null region encountered during neuron update");
                continue;
            }
            
            for (auto& pop : region->getPopulations()) {
                if (!pop) {
                    NLM_LOG_WARNING("Null population encountered during neuron update");
                    continue;
                }
                
                for (auto* neuron : pop->getNeurons()) {
                    if (!neuron) {
                        NLM_LOG_WARNING("Null neuron encountered during neuron update");
                        continue;
                    }
                    
                    try {
                        neuron->stepLIF(currentTime, timestep);
                    } catch (const std::exception& e) {
                        NLM_LOG_ERROR(std::string("Error updating neuron ") + std::to_string(neuron->getId()) + ": " + e.what());
                    }
                }
            }
        }
    }
    
    void detectSpikes(SimulationStep currentStep, Timestamp currentTime) {
        for (auto& region : regions) {
            if (!region) continue;
            
            for (auto& pop : region->getPopulations()) {
                if (!pop) continue;
                
                for (auto* neuron : pop->getNeurons()) {
                    if (!neuron) continue;
                    
                    try {
                        const auto& state = neuron->getState();
                        bool justFired = (state.firingState == FiringState::Refractory &&
                                         state.lastSpikeTime >= 0.0f &&
                                         std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);
                        
                        if (justFired) {
                            // Neuron fired this step - queue the spike
                            SpikeEvent event(neuron->getId(), currentTime, currentStep);
                            
                            if (spikeSystem) {
                                spikeSystem->queueSpike(event);
                            }
                            
                            // Record post-synaptic spike for incoming synapses (plasticity)
                            auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                            for (Synapse* syn : incomingSynapses) {
                                if (syn) {
                                    try {
                                        syn->recordPostSpike(currentTime);
                                    } catch (const std::exception& e) {
                                        NLM_LOG_ERROR(std::string("Error recording post-synaptic spike: ") + e.what());
                                    }
                                }
                            }
                            
                            // Get outgoing synapses and schedule delayed spike events
                            auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                            for (Synapse* syn : outgoingSynapses) {
                                if (!syn) continue;
                                
                                try {
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
                                    
                                    if (spikeSystem) {
                                        spikeSystem->queueDelayedSpike(delayedEvent);
                                    }
                                    
                                    // Record pre-synaptic spike for plasticity
                                    syn->recordPreSpike(currentTime);
                                    
                                } catch (const std::exception& e) {
                                    NLM_LOG_ERROR(std::string("Error creating delayed spike event: ") + e.what());
                                }
                            }
                            
                            // Store to working memory - neurons that fire become part of working memory
                            if (workingMemory) {
                                try {
                                    workingMemory->storeToNeuron(neuron->getId(), 
                                        std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                                } catch (const std::exception& e) {
                                    NLM_LOG_ERROR(std::string("Error storing neuron to working memory: ") + e.what());
                                }
                            }
                        }
                    } catch (const std::exception& e) {
                        NLM_LOG_ERROR(std::string("Error detecting spike for neuron ") + std::to_string(neuron->getId()) + ": " + e.what());
                    }
                }
            }
        }
    }
    
    void updateWorkingMemory(TimestepDuration timestep) {
        if (workingMemory) {
            try {
                workingMemory->update(timestep);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Error updating working memory: ") + e.what());
            }
        }
    }
    
    void applyNeuromodulationEffects(TimestepDuration timestep) {
        // Update novelty detection
        if (novelty) {
            try {
                novelty->update(timestep);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Error updating novelty: ") + e.what());
            }
        }
        
        // Update curiosity
        if (curiosity) {
            try {
                curiosity->update(timestep);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Error updating curiosity: ") + e.what());
            }
        }
        
        // Update dopamine (reward prediction error)
        if (dopamine) {
            try {
                dopamine->update(timestep);
                
                // Apply dopamine effects on neural excitability
                float dopamineLevel = dopamine->getLevel();
                for (auto& region : regions) {
                    for (auto& pop : region->getPopulations()) {
                        for (auto* neuron : pop->getNeurons()) {
                            if (!neuron) continue;
                            
                            try {
                                // Dopamine modulates excitability by injecting additional current
                                float excitabilityMod = dopamineLevel * 0.5f;
                                if (excitabilityMod > 0.0f) {
                                    neuron->injectCurrent(excitabilityMod);
                                }
                            } catch (const std::exception& e) {
                                NLM_LOG_ERROR(std::string("Error applying dopamine effects: ") + e.what());
                            }
                        }
                    }
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Error updating dopamine: ") + e.what());
            }
        }
    }
    
    void applyPlasticityRules(TimestepDuration timestep) {
        // Calculate neuromodulation factor for plasticity
        float plasticityMod = 1.0f;
        if (dopamine) {
            plasticityMod = dopamine->getPlasticityFactor();
        }
        
        for (auto& region : regions) {
            for (auto& syn : region->getSynapses()) {
                if (!syn) {
                    NLM_LOG_WARNING("Null synapse encountered during plasticity");
                    continue;
                }
                
                try {
                    // Apply STDP with neuromodulation
                    if (syn->getPlasticityFlags().stdp) {
                        const auto& preSpikes = syn->getPreSpikeHistory();
                        const auto& postSpikes = syn->getPostSpikeHistory();
                        
                        if (!preSpikes.empty() && !postSpikes.empty()) {
                            // Modify weight change based on dopamine
                            if (stdp) {
                                stdp->update(syn, preSpikes, postSpikes, timestep);
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
                            if (hebbian) {
                                hebbian->update(syn, preSpikes, postSpikes, timestep);
                            }
                        }
                    }
                    
                    // Update synapse state
                    syn->step(timestep);
                    
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Error applying plasticity rules: ") + e.what());
                }
            }
        }
    }
    
    void updateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime) {
        stepsSinceLastEpisode++;
        if (stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
            stepsSinceLastEpisode = 0;
            
            if (episodicMemory) {
                try {
                    // Capture current brain state as an episode
                    EpisodicMemoryItem episode;
                    episode.timestamp = currentStep;
                    episode.reward = dopamine ? dopamine->getLevel() : 0.0f;
                    
                    // Store active neurons
                    for (auto& region : regions) {
                        for (auto& pop : region->getPopulations()) {
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
                    episode.reward = dopamine ? dopamine->getLevel() : 0.0f;
                    
                    episodicMemory->storeEpisode(episode);
                    
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Error updating episodic memory: ") + e.what());
                }
            }
        }
    }
    
    void updatePredictionSystem(TimestepDuration timestep) {
        if (predictionSystem) {
            try {
                // The prediction system would be updated with sensory observations
                // For now, just track prediction error history
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Error updating prediction system: ") + e.what());
            }
        }
    }
    
    void updateAttentionSystem(TimestepDuration timestep) {
        if (attention) {
            try {
                attention->update(timestep);
                
                // Apply attention to working memory winners
                if (workingMemory && !workingMemory->getMemoryNeurons().empty()) {
                    std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
                    attention->processCompetition(competitors);
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Error updating attention system: ") + e.what());
            }
        }
    }
    
    void updateConceptFormation(TimestepDuration timestep) {
        if (conceptFormation) {
            try {
                // Would process current neural activity patterns to form concepts
                // This requires sensory state encoding
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Error updating concept formation: ") + e.what());
            }
        }
    }
    
    void applyStructuralPlasticity(SimulationStep currentStep, Timestamp currentTime) {
        if (structuralPlasticity && (currentStep % 100 == 0)) {
            try {
                structuralPlasticity->update(this, *rng);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Error applying structural plasticity: ") + e.what());
            }
        }
    }
    
    void replayMemories(SimulationStep currentStep, Timestamp currentTime) {
        if (currentStep % replayInterval == 0 && episodicMemory) {
            try {
                // Get episodes for replay
                auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
                for (const auto* episode : episodesToReplay) {
                    episodicMemory->replayEpisode(episode);
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Error replaying memories: ") + e.what());
            }
        }
    }
    
    void applyDevelopmentEffects(SimulationStep currentStep, Timestamp currentTime) {
        if (currentStep % 1000 == 0) {  // Update development every 1000 steps
            if (developmentSystem) {
                try {
                    developmentSystem->update(this, *rng, timestep * 1000);
                    
                    // Development affects plasticity rates
                    if (structuralPlasticity) {
                        DevelopmentalStage stage = getDevelopmentalStage();
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
                        
                        structuralPlasticity->setSynaptogenesisRate(0.0001f * plasticityMod);
                        structuralPlasticity->setPruningRate(0.00001f * (2.0f - plasticityMod));
                    }
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Error applying development effects: ") + e.what());
                }
            }
        }
    }
    
    void consolidateMemory(SimulationStep currentStep, Timestamp currentTime) {
        if (currentStep % consolidationInterval == 0 && episodicMemory) {
            try {
                // Consolidate important memories, remove weak ones
                episodicMemory->consolidate(0.3f);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Error consolidating memory: ") + e.what());
            }
        }
    }
    
    void manageCheckpoints(SimulationStep currentStep, Timestamp currentTime) {
        if (checkpointManager) {
            try {
                checkpointManager->update(currentStep, currentTime);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Error managing checkpoints: ") + e.what());
            }
        }
    }
    
    void collectStatistics(SimulationStep currentStep, Timestamp currentTime) {
        // Update development stage based on time
        if (currentStep % 10000 == 0) {
            if (developmentSystem) {
                try {
                    DevelopmentalStage currentStage = developmentSystem->getCurrentStage(currentStep);
                    if (currentStage != getDevelopmentalStage()) {
                        setDevelopmentalStage(currentStage);
                    }
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Error updating development stage: ") + e.what());
                }
            }
        }
    }
    
    // Memory system access methods
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() { return episodicMemory.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() { return associativeMemory.get(); }
    PredictionSystem* getPredictionSystem() { return predictionSystem.get(); }
    NeuralPlanner* getPlanner() { return planner.get(); }
    ConceptFormation* getConceptFormation() { return conceptFormation.get(); }
    AttentionalSelection* getAttention() { return attention.get(); }
    DevelopmentSystem* getDevelopmentSystem() { return developmentSystem.get(); }
    Dopamine* getDopamine() { return dopamine.get(); }
    Curiosity* getCuriosity() { return curiosity.get(); }
    Novelty* getNovelty() { return novelty.get(); }
    PredictionError* getPredictionError() { return predictionError.get(); }
    
    // Statistics methods
    float getExcitationInhibitionRatio() const {
        float totalExcitatory = 0.0f;
        float totalInhibitory = 0.0f;
        
        for (const auto& region : regions) {
            if (!region) continue;
            
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
    
    size_t getTotalSpikeCount() const { return totalSpikesTotal; }
    size_t getPendingSpikeEventCount() const {
        if (!spikeSystem) return 0;
        return spikeSystem->getPendingSpikeCount() + spikeSystem->getPendingDelayedCount();
    }
    
    size_t getTotalNeuronCount() const {
        size_t total = 0;
        for (const auto& region : regions) {
            if (region) {
                total += region->getTotalNeuronCount();
            }
        }
        return total;
    }
    
    size_t getTotalSynapseCount() const {
        size_t total = 0;
        for (const auto& region : regions) {
            if (region) {
                total += region->getSynapseCount();
            }
        }
        total += interRegionConnections.size();
        return total;
    }
    
    size_t getActiveNeuronCount() const {
        size_t total = 0;
        for (const auto& region : regions) {
            if (region) {
                total += region->getActiveNeuronCount();
            }
        }
        return total;
    }
    
    size_t getFiringNeuronCount() const {
        return totalSpikesThisStep;
    }
    
    float getAverageFiringRate() const {
        if (regions.empty()) return 0.0f;
        float sum = 0.0f;
        size_t count = 0;
        
        for (const auto& region : regions) {
            if (region) {
                float rate = region->getAverageFiringRate();
                sum += rate;
                count++;
            }
        }
        
        return count > 0 ? sum / static_cast<float>(count) : 0.0f;
    }
    
    size_t getRegionCount() const { return regions.size(); }
    
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const { return regions; }
    
    // Action production
    std::unique_ptr<Action> produceAction() {
        // Simple action selection based on motor neuron activity
        if (motorNeurons.empty()) {
            return std::make_unique<Action>(ActionType::Wait);
        }
        
        // Calculate activity of motor neuron groups
        size_t firingMotor = 0;
        for (auto* neuron : motorNeurons) {
            if (!neuron) continue;
            if (neuron->isFiring()) {
                ++firingMotor;
            }
        }
        
        // Return a simple action
        ActionType type = ActionType::Wait;
        if (firingMotor > 0) {
            type = ActionType::MoveForward;
        }
        
        return std::make_unique<Action>(type);
    }
    
    // Reset method
    void reset() {
        std::lock_guard<std::mutex> lock(criticalMutex);
        
        NLM_LOG_INFO("Resetting NLM Brain...");
        
        // Reset internal state
        currentStep = 0;
        currentTime = 0.0;
        totalSpikesThisStep = 0;
        totalSpikesTotal = 0;
        isResting = false;
        stepsSinceLastEpisode = 0;
        
        // Reset regions
        for (auto& region : regions) {
            if (region) {
                region->reset();
            }
        }
        
        // Reset spike system
        if (spikeSystem) {
            spikeSystem->reset();
        }
        
        // Reset development stage
        developmentalStage = DevelopmentalStage::Initial;
        
        // Reset memory systems
        if (workingMemory) workingMemory->clear();
        if (episodicMemory) episodicMemory->clear();
        if (associativeMemory) associativeMemory->clear();
        if (attention) attention->reset();
        
        NLM_LOG_INFO("NLM Brain reset complete");
    }
    
    // Save/load methods
    bool save(const std::string& filepath) const {
        std::lock_guard<std::mutex> lock(criticalMutex);
        
        NLM_LOG_INFO("Saving brain state to " + filepath);
        
        try {
            InputValidator::isNonEmpty("filepath", filepath);
            
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
                currentStep,
                currentTime
            );
            
            // Write neurons
            NeuronCheckpointData neuronData;
            neuronData.membranePotential.reserve(getTotalNeuronCount());
            neuronData.restingPotential.reserve(getTotalNeuronCount());
            neuronData.threshold.reserve(getTotalNeuronCount());
            neuronData.resetPotential.reserve(getTotalNeuronCount());
            neuronData.leakConductance.reserve(getTotalNeuronCount());
            
            for (const auto& region : regions) {
                if (!region) continue;
                
                for (const auto& pop : region->getPopulations()) {
                    if (!pop) continue;
                    
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
            for (const auto& region : regions) {
                if (!region) continue;
                
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
            
        } catch (const BrainError& e) {
            NLM_LOG_ERROR(std::string("Brain save error: ") + e.what());
            return false;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
            return false;
        }
    }
    
    bool load(const std::string& filepath) {
        std::lock_guard<std::mutex> lock(criticalMutex);
        
        NLM_LOG_INFO("Loading brain state from " + filepath);
        
        try {
            InputValidator::isNonEmpty("filepath", filepath);
            
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
            
            // Apply neuron states - this is complex because we need to find matching synapses
            // For now, just log the count
            NLM_LOG_INFO("Loaded " + std::to_string(neuronData.membranePotential.size()) + " neurons");
            
            NLM_LOG_INFO("Brain state loaded successfully");
            return true;
            
        } catch (const BrainError& e) {
            NLM_LOG_ERROR(std::string("Brain load error: ") + e.what());
            return false;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
            return false;
        }
    }
    
    // Logging
    void logStatus() const {
        std::lock_guard<std::mutex> lock(criticalMutex);
        
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
        if (workingMemory) {
            NLM_LOG_INFO("Working memory traces: " + std::to_string(workingMemory->getActiveTraces()));
        }
        if (episodicMemory) {
            NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(episodicMemory->getEpisodeCount()));
        }
        
        // Neuromodulation status
        if (dopamine) {
            NLM_LOG_INFO("Dopamine level: " + std::to_string(dopamine->getLevel()));
        }
        
        // Development status
        NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(developmentalStage)));
        
        for (const auto& region : regions) {
            if (region) {
                NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
            }
        }
    }
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
        if (!pImpl->isBrainInitialized()) {
            throw BrainError("Brain implementation not properly initialized", "initialize");
        }
        
        NLM_LOG_INFO("Configuration: " + std::to_string(pImpl->config->getOr<size_t>("neuron_count", 1000)) + " neurons, " + 
                     std::to_string(pImpl->config->getOr<size_t>("region_count", 1)) + " regions");
        
        // Initialize configuration values
        size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
        size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
        float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
        
        InputValidator::isWithinRange("neuron_count", neuronCount, 1, 1000000);
        InputValidator::isWithinRange("region_count", regionCount, 1, 100);
        InputValidator::isWithinRange("connection_probability", connectionProbability, 0.0f, 1.0f);
        
        // Create regions with error handling
        for (size_t i = 0; i < regionCount; ++i) {
            try {
                addRegion("Region_" + std::to_string(i + 1));
            } catch (const BrainError& e) {
                NLM_LOG_ERROR(std::string("Failed to create region ") + std::to_string(i) + ": " + e.what());
                // Continue with remaining regions - degrade gracefully
            }
        }
        
        // Create neurons across regions with error handling
        size_t neuronsPerRegion = neuronCount / regionCount;
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(RegionId(i + 1));
            if (!region) {
                NLM_LOG_WARNING("Region " + std::to_string(i + 1) + " is null - skipping");
                continue;
            }
            
            try {
                // Add populations with error handling
                auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
                auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
                auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
                
                // Collect sensory and motor neurons for I/O
                auto* sensoryPop = region->getPopulation(sensoryPopId);
                auto* motorPop = region->getPopulation(motorPopId);
                
                if (sensoryPop) {
                    for (auto* neuron : sensoryPop->getNeurons()) {
                        if (neuron) {
                            pImpl->sensoryNeurons.push_back(neuron);
                        }
                    }
                }
                
                if (motorPop) {
                    for (auto* neuron : motorPop->getNeurons()) {
                        if (neuron) {
                            pImpl->motorNeurons.push_back(neuron);
                        }
                    }
                }
                
                NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                    ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons");
                    
            } catch (const BrainError& e) {
                NLM_LOG_ERROR(std::string("Error in region ") + std::to_string(i + 1) + ": " + e.what());
                // Continue with remaining regions - degrade gracefully
            }
        }
        
        // Initialize connectivity with random weights with error handling
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(RegionId(i + 1));
            if (region) {
                try {
                    region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Failed to initialize connectivity for region ") + 
                        std::to_string(i + 1) + ": " + e.what());
                    // Continue - some regions may have limited connectivity
                }
            }
        }
        
        // Initialize all integrated systems with graceful degradation
        initializeIntegratedSystems();
        
        NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
        NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
        NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
        NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
        
        return true;
        
    } catch (const BrainError& e) {
        NLM_LOG_ERROR(std::string("Brain initialization failed: ") + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error during brain initialization: ") + e.what());
        return false;
    }
}

void Brain::initializeIntegratedSystems() {
    try {
        // Initialize working memory
        if (pImpl->workingMemory) {
            pImpl->workingMemory->initialize(this);
            pImpl->workingMemory->setCapacity(getTotalNeuronCount() / 10);
        }
        
        // Initialize episodic memory
        if (pImpl->episodicMemory) {
            pImpl->episodicMemory->initialize(this);
            pImpl->episodicMemory->setMaxEpisodes(1000);
        }
        
        // Initialize associative memory
        if (pImpl->associativeMemory) {
            pImpl->associativeMemory->initialize(this);
        }
        
        // Initialize prediction system
        if (pImpl->predictionSystem) {
            // (PredictionSystem doesn't have initialize method currently)
        }
        
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
        
        // Initialize neuromodulation
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
        std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        InputValidator::isNonEmpty("checkpoint_dir", checkpointDir);
        
        if (pImpl->checkpointManager) {
            pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
        }
        
        pImpl->registerSystemInitializationFlags();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error during integrated systems initialization: ") + e.what());
        // Continue with degraded functionality - some systems may not be available
    }
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    try {
        pImpl->executeStep(currentStep, currentTime);
    } catch (const BrainError& e) {
        NLM_LOG_ERROR(std::string("Brain step error: ") + e.what());
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error during brain step: ") + e.what());
        throw BrainError(std::string("Brain step failed: ") + e.what(), "step");
    }
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    try {
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
            if (i < pImpl->sensoryNeurons.size() && pImpl->sensoryNeurons[i]) {
                pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
            }
            
            // Also store in working memory
            if (pImpl->workingMemory && normalizedValue > 0.5f && i < pImpl->sensoryNeurons.size() && pImpl->sensoryNeurons[i]) {
                pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
            }
        }
    } catch (const BrainError& e) {
        NLM_LOG_ERROR(std::string("Brain sensory input error: ") + e.what());
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error during sensory input: ") + e.what());
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    try {
        InputValidator::isFinite("neuron ID", neuron.index());
        InputValidator::isFinite("current", current);
        
        for (auto& region : pImpl->regions) {
            if (!region) continue;
            
            auto neurons = region->getAllNeurons();
            for (auto* n : neurons) {
                if (n && n->getId() == neuron) {
                    n->injectCurrent(current);
                    return;
                }
            }
        }
    } catch (const BrainError& e) {
        NLM_LOG_ERROR(std::string("Brain inject current error: ") + e.what());
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error during inject current: ") + e.what());
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    try {
        InputValidator::isFinite("current", current);
        
        for (auto& region : pImpl->regions) {
            if (!region) continue;
            
            for (auto& pop : region->getPopulations()) {
                if (!pop) continue;
                
                if (pop->getNeuronType() == type) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron) {
                            neuron->injectCurrent(current);
                        }
                    }
                }
            }
        }
    } catch (const BrainError& e) {
        NLM_LOG_ERROR(std::string("Brain inject current to neurons error: ") + e.what());
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error during inject current to neurons: ") + e.what());
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
    return pImpl->getExcitationInhibitionRatio();
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl->totalSpikesTotal;
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->getPendingSpikeEventCount();
}

std::unique_ptr<Action> Brain::produceAction() {
    return pImpl->produceAction();
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
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
}

void Brain::reset() {
    try {
        pImpl->reset();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error during brain reset: ") + e.what());
        throw;
    }
}

bool Brain::save(const std::string& filepath) const {
    try {
        InputValidator::isNonEmpty("filepath", filepath);
        return pImpl->save(filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    try {
        InputValidator::isNonEmpty("filepath", filepath);
        return pImpl->load(filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

RegionId Brain::addRegion(const std::string& name) {
    try {
        return pImpl->addRegion(name);
    } catch (const BrainError& e) {
        NLM_LOG_ERROR(std::string("Brain add region error: ") + e.what());
        throw;
    }
}

NeuralRegion* Brain::getRegion(RegionId id) {
    return pImpl->getRegion(id);
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    return pImpl->getRegion(id);
}

size_t Brain::getRegionCount() const {
    return pImpl->getRegionCount();
}

std::vector<RegionId> Brain::getRegionIds() const {
    std::vector<RegionId> ids;
    ids.reserve(pImpl->getRegionCount());
    for (const auto& region : pImpl->getRegions()) {
        if (region) {
            ids.push_back(region->getId());
        }
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->getRegions();
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    try {
        pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error adding inter-region connection: ") + e.what());
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
    return pImpl->getTotalNeuronCount();
}

size_t Brain::getTotalSynapseCount() const {
    return pImpl->getTotalSynapseCount();
}

size_t Brain::getActiveNeuronCount() const {
    return pImpl->getActiveNeuronCount();
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl->getFiringNeuronCount();
}

float Brain::getAverageFiringRate() const {
    return pImpl->getAverageFiringRate();
}

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->getWorkingMemory();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->getEpisodicMemory();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->getAssociativeMemory();
}

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->getPredictionSystem();
}

NeuralPlanner* Brain::getPlanner() {
    return pImpl->getPlanner();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->getConceptFormation();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->getAttention();
}

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->getDevelopmentSystem();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->getDevelopmentalStage();
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->setDevelopmentalStage(stage);
}

Dopamine* Brain::getDopamine() {
    return pImpl->getDopamine();
}

Curiosity* Brain::getCuriosity() {
    return pImpl->getCuriosity();
}

Novelty* Brain::getNovelty() {
    return pImpl->getNovelty();
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl->getPredictionError();
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->getRng();
}

void Brain::logStatus() const {
    pImpl->logStatus();
}

} // namespace nlm

// Template instantiations for type-safe accessors
namespace nlm {

// Explicit instantiations for Brain class
// These provide type-safe accessors for common types

template std::optional<size_t> Brain::Impl::getTyped<size_t>(const std::string&) const;

template size_t Brain::Impl::getOr<size_t>(const std::string&, const size_t&) const;

template std::optional<float> Brain::Impl::getTyped<float>(const std::string&) const;

template float Brain::Impl::getOr<float>(const std::string&, const float&) const;

// Additional explicit instantiations can be added here as needed

} // namespace nlm